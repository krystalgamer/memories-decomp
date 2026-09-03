# Runtime Overlay and MRG Loader Trace

## Repository representation

The resident executable reserves `0x8013A000-0x801AC000` for runtime-loaded
content. Its original mostly zero-filled image is split into address-based
assets below `tmp/splat/assets/overlays/`:

| Asset | VRAM range | Observed runtime source |
|---|---|---|
| `slot_8013a000.bin` | `0x8013A000-0x80146000` | MODEL slot A |
| `slot_80146000.bin` | `0x80146000-0x80168000` | WA shared duel bank |
| `slot_80168000.bin` | `0x80168000-0x8017A000` | WA screen bank |
| `slot_8017a000.bin` | `0x8017A000-0x80180000` | MODEL slot B and WA table data |
| `slot_80180000.bin` | `0x80180000-0x801AC000` | SU bank |

The address names keep uncertain inner boundaries out of build metadata while
separating the five independently observed load banks from resident assets.

These binaries are not the overlay code itself. The executable reconstructs
them byte-for-byte, then replaces portions of those addresses at runtime with
payloads read from `WA_MRG.MRG`, `MODEL.MRG`, and `SU.MRG`. Those archive
payloads remain research inputs rather than tracked build sources.

## Result

The resident executable has a two-descriptor asynchronous CD loader that reads
logical `0x800`-byte sectors from the seven runtime files. Its high-memory
destination table is shared among WA, MODEL, and SU content.

High-confidence archive attribution is:

| Destination | Archive | Evidence |
|---:|---|---|
| `0x80146000` | WA | Seven identical WA chunks; valid code at resident call target `0x801462B0` |
| `0x80168000` | WA | Multiple WA packages; valid code at several resident `0x80168xxx` call targets |
| `0x8017A1D8` | WA | Identical five-sector data subrange in seven WA records |
| `0x8013A000` | MODEL | Index-two callback writes the exact base; game calls base `+4` |
| `0x8017A000` | MODEL | MODEL supplies executable content at the base; WA writes only at `+0x1D8` |
| `0x80180000` | SU | Two index-one loads produce valid code at resident `0x80180xxx` call targets |

No resident WA path was found writing `0x8013A000` or `0x80180000`.
Dynamically loaded code could issue further reads, but the resident
executable does not support treating all slots as WA-owned.

## File lookup and runtime layout

The null-terminated filename pointer table begins at `0x8009078C`:

| Index | File | Runtime start LBA |
|---:|---|---:|
| 0 | `WA_MRG.MRG` | 10,102 |
| 1 | `SU.MRG` | 954 |
| 2 | `MODEL.MRG` | 28,534 |
| 3 | `MOVIE.STR` | 202,734 |
| 4 | `SD_SE.DAT` | 2,193 |
| 5 | `SD_BGM.DAT` | 2,936 |
| 6 | `MASTER.XA` | 199,930 |

`func_800136E4` enumerates these names. `func_800138F4` repeatedly performs the
`CdSearchFile`-like lookup and converts BCD MSF to a zero-based LBA with
`func_8007E710`.

The BSS layout is:

```text
0x800E9E18-0x800E9E5F  primary transfer descriptor, 0x48 bytes
0x800E9E60-0x800E9EA7  secondary transfer descriptor, 0x48 bytes
0x800E9EA8-0x800E9EC3  seven runtime LBA words
```

The corrected LBA-table address is `0x800E9EA8`; interpreting the signed
`addiu` immediate as unsigned incorrectly produces `0x800F9EA8`.

## Transfer descriptor

Observed fields in each `0x48`-byte descriptor are:

| Offset | Width | Behavior |
|---:|---:|---|
| `+0x00/+0x02` | 16-bit | GPU rectangle or transfer header |
| `+0x04/+0x06` | 16-bit | GPU rectangle or transfer header |
| `+0x08` | pointer | Current/direct sector destination |
| `+0x0C` | pointer | Alternate or end destination |
| `+0x10` | 32-bit | Total transfer bytes |
| `+0x14` | 32-bit | File-relative byte offset |
| `+0x18` | 32-bit | Public loader argument; exact meaning unknown |
| `+0x1C` | 32-bit | Current phase byte count |
| `+0x20` | pointer | Phase callback `(descriptor, phase_index)` |
| `+0x24` | 32-bit | Absolute logical disc LBA |
| `+0x28` | 32-bit | Current phase bytes remaining |
| `+0x2C` | 32-bit | File index in low nibble plus transfer flags |
| `+0x30/+0x32` | 16-bit | GPU or partial-transfer metadata |
| `+0x34` | 32-bit | Optional direct destination/configuration |
| `+0x38/+0x3C` | 32-bit | Caller-specific callback metadata |
| `+0x40` | 32-bit | Phase index or seed |
| `+0x44` | 16-bit | Alternating-buffer index |
| `+0x46` | 8-bit | Transfer state |
| `+0x47` | 8-bit | Transfer substate |

`func_80013940` applies:

```c
file_index                = file_flags & 0xF;
descriptor->file_bytes    = sector_offset << 11;
descriptor->absolute_lba  = D_800E9EA8[file_index] + sector_offset;
descriptor->total_bytes   = abs(sector_count << 11);
```

`func_80013998` initializes a descriptor. `func_80014E1C` is the common
game-facing asynchronous loader using the secondary descriptor. Its relevant
arguments are the file selector, file-relative sector offset, sector count,
phase callback, phase seed, and optional direct destination.

`func_8001455C` services the CD transfer. `func_80013C28` consumes one sector
at a time through the `CdGetSector`-like function at `0x8007E3D0`, advances
the destination by `0x800`, and invokes the phase callback when the phase byte
count reaches zero. `func_800137E4` is the synchronous wait used by many
callers.

## Shared high-memory table

The table at `0x80010000` contains:

| Index | Value | Observed role |
|---:|---:|---|
| 0 | `0x80100000` | Main WA callback bank |
| 1 | `0x80140000` | Second main bank |
| 2 | `0x80180000` | Third main bank |
| 3 | `0x8013A000` | Paired executable/module slot A |
| 4 | `0x8017A000` | Paired executable/module slot B |
| 5 | `0x8013B000` | Paired slot A |
| 6 | `0x8017B000` | Paired slot B |
| 7 | `0x80135000` | Paired work/data slot A |
| 8 | `0x80175000` | Paired work/data slot B |
| 9 | `0x80136000` | Paired work/data slot A |
| 10 | `0x80176000` | Paired work/data slot B |
| 11 | `0x80180000` | SU callback destination |
| 12 | `0x80180000` | SU direct destination |
| 13 | `0x80185CD4` | Pointer used after an SU overlay load |

Game code selects indices 3 and 4 as callable module bases, indices 11 and 12
for SU transfers, and index 13 after loading an SU module. This is a shared
layout table, not an archive directory.

A separate two-word table at `0x800101D8` is WA-specific in all observed
resident references:

```text
0x800101D8 -> 0x80168000
0x800101DC -> 0x80146000
```

## Recovered WA sector layout

WA occupies 18,432 logical sectors. No conventional resident offset array was
found. The recovered index is compiled as immediate sector offsets, affine
stride calculations, phase callbacks, and byte counts.

The following packages tile WA sectors `5776-8661` without gaps:

| Package | WA sector range | Count | WA byte range |
|---|---|---:|---|
| Fixed package | `5776-5830` | 54 | `0xB48000-0xB63000` |
| Seven records | `5830-7475` | `7 * 235` | `0xB63000-0xE99800` |
| Mini-record family | `7475-7595` | inferred `40 * 3` | `0xE99800-0xED5800` |
| Fixed package | `7595-7629` | 34 | `0xED5800-0xEE6800` |
| Fixed package | `7629-7767` | 138 | `0xEE6800-0xF2B800` |
| Fixed package | `7767-7816` | 49 | `0xF2B800-0xF44000` |
| Fixed package | `7816-7903` | 87 | `0xF44000-0xF6F800` |
| Fixed package | `7903-7983` | 80 | `0xF6F800-0xF97800` |
| Fixed package | `7983-8069` | 86 | `0xF97800-0xFC2800` |
| Fixed package | `8069-8103` | 34 | `0xFC2800-0xFD3800` |
| Fixed package | `8103-8153` | 50 | `0xFD3800-0xFEC800` |
| Variant A | `8153-8311` | 158 | `0xFEC800-0x103B800` |
| Variant B | `8311-8469` | 158 | `0x103B800-0x108A800` |
| Fixed package | `8469-8519` | 50 | `0x108A800-0x10A3800` |
| Direct bank load | `8519-8535` | 16 | `0x10A3800-0x10AB800` |
| Fixed package | `8535-8585` | 50 | `0x10AB800-0x10C4800` |
| Fixed package | `8585-8661` | 76 | `0x10C4800-0x10EA800` |

The next loader family begins from sector 8661, reinforcing the interpretation
of these values as generated merge boundaries.

## Seven 235-sector WA records

`func_8001798C` and `func_800179F4` calculate:

```text
record sector = 5830 + selected_index * 235
record count  = 235 sectors
callback      = func_800171A8
```

The following independently indexed family begins at sector 7475:

```text
7475 - 5830 = 1645 = 7 * 235
```

This proves seven physical records, although the semantic identity of indices
zero through six is not yet known.

The 13 callback phases are:

| Phase | Relative sectors | Count | Bytes | Destination or mode |
|---:|---:|---:|---:|---|
| 0 | `0-64` | 64 | `0x20000` | GPU/VRAM path |
| 1 | `64-68` | 4 | `0x2000` | Streaming buffer |
| 2 | `68-73` | 5 | `0x2800` | `0x8017A1D8` |
| 3 | `73-105` | 32 | `0x10000` | `0x8017C2D8` |
| 4 | `105-106` | 1 | `0x800` | `0x801799D8` |
| 5 | `106-108` | 2 | `0x1000` | Streaming buffer |
| 6 | `108-140` | 32 | `0x10000` | GPU/VRAM path |
| 7 | `140-184` | 44 | `0x16000` | `0x80146000` |
| 8 | `184-187` | 3 | `0x1800` | `0x801A8000` |
| 9 | `187-190` | 3 | `0x1800` | `0x801A9800` |
| 10 | `190-198` | 8 | `0x4000` | GPU/VRAM path |
| 11 | `198-203` | 5 | `0x2800` | `0x80100000` |
| 12 | `203-235` | 32 | `0x10000` | GPU/VRAM path |

The counts sum to 235 exactly.

### WA at `0x80146000`

Phase 7 of every record copies 44 sectors to `0x80146000`. All seven chunks
are byte-identical:

```text
size:       0x16000
first word: 0x00000018
SHA-256:    baa203b937dc6bdf91b1826c5832f0f32e11ae5fe9d05193a4361bc08158b9e0
```

The first record's phase begins at WA sector 5970. At destination offset
`+0x2B0`, the bytes decode as a normal function at `0x801462B0`, exactly where
resident code calls.

### WA at `0x80168000`

Several fixed packages end with phases directed to `0x80168000`:

| WA sectors | Size | Confirmed called code |
|---|---:|---|
| `5827-5830` | `0x1800` | `0x801680F4`, `0x80168160` |
| `7898-7903` | `0x2800` | `0x8016824C` |
| `7968-7983` | `0x7800` | `0x8016AA6C` |
| `8054-8069` | `0x7800` | `0x8016A080`, `0x8016A37C` |
| `8153-8159` | `0x3000` | `0x8016866C`, `0x80168FCC` |
| `8311-8317` | `0x3000` | Same callback family, alternate variant |

The chunks begin with module-like identifiers from `0x13` through `0x16`.
Each checked resident call target contains valid MIPS code at the exact loaded
offset.

### WA data at `0x8017A1D8`

Phase 2 of each 235-sector record copies five sectors to `0x8017A1D8`.
All seven data chunks are byte-identical:

```text
size:       0x2800
SHA-256:    0b99ccb2d4c2ccf3e0fd41b6d35183294ab995f2b2b631b442e6673b5a7352ac
```

Resident code reads this exact address. The transfer overlaps the next phase's
destination by `0x700` bytes, likely because the merged input is
sector-rounded; the original unrounded size is unknown.

MODEL, not WA, supplies executable content at the `0x8017A000` base.

## MODEL at `0x8013A000`

Game code loads the table entry at `0x8001000C` and calls base `+4`. The
resident transfer callback that selects `0x8013A000` is reached from a loader
call with file index 2, which is `MODEL.MRG`.

No resident index-zero callback or direct WA transfer was found for
`0x8013A000` or `0x8013A1D8`.

## SU at `0x80180000`

Two resident index-one loads target `0x80180000`:

| SU sectors | Size | First word | Loader form |
|---|---:|---:|---|
| `98-114` | `0x8000` | `0x0000000F` | Phase of SU sectors `0-115` |
| `1223-1239` | `0x8000` | `0x00000010` | Direct destination |

The first chunk contains valid code at resident call targets including:

```text
0x8018001C
0x80180390
0x801807B0
0x80180DD0
0x80181C4C
0x80181F68
0x801821DC
0x80183FE4
```

No resident WA callback was found targeting table indices 11 or 12.

## Disc-image validation

Every focused WA and SU range used for attribution was checked sector by sector
against the original MODE2/2352 BIN. For an archive-relative sector:

```text
disc LBA         = archive start LBA + archive sector
raw sector start = disc LBA * 0x930
raw user data    = raw sector start + 0x18
```

All compared `0x800`-byte user-data payloads matched the extracted MRG files.
The disc was not modified or rebuilt.

## Confidence and remaining unknowns

High-confidence results include the file indices, corrected LBA-table address,
descriptor sizes, sector units, WA boundaries from sectors 5776 through 8661,
the seven-record phase layout, archive-to-slot attribution, and code at exact
resident call targets.

The following remain provisional:

- Whether the leading words are formally named module IDs.
- The original source/header name for the WA destination table.
- The semantic identities of the seven 235-sector records and two 158-sector
  variants.
- Original unrounded inner-file sizes.
- Full MODEL and SU merged-file manifests.
- Reads that might be issued by dynamically loaded code rather than resident
  code.

## Semantic identities, from the disc side

Measured in the `MaChInEgUn3/ygofm-decomp` tree against the same NTSC-U
disc; the layout above stands as written, this only fills some of its
provisional items.

**The seven 235-sector records are the seven terrain types.** The index in
`func_8001798C` is the terrain byte `D_8009B364` (0 normal, 1 forest, 2
wasteland, 3 mountain, 4 meadow, 5 sea, 6 dark). Hashing all thirteen phases
across the seven records, twelve are byte-identical and only phase 12 (the
last `0x10000`, VRAM (640, 256)) differs — the field picture. So the seven
copies are seven backgrounds each shipped with a duplicate of the same 416
KB.

**Phases 2, 3 and 4 are the equip, fusion and ritual tables**, and they
decode with every card id in range: 4,041 (equip, monster) pairs over the 34
equip cards; 25,131 fusion recipes indexed by the smaller card id through a
`u16 offset[723]` table; 24 rituals. The equip table's real length is
`0x201C` bytes including its zero terminator, which is why the sector-rounded
`0x2800` phase overlaps the fusion destination by `0x700` without harm.

**The two 158-sector variants (WA 8153 and 8311) are the Egypt overworld
before and after Heishin's coup.** `func_8003C0C0` picks `0x1FD9` or
`0x1FD9 + 0x9E` on save flag `0x47`, which the dialogue "Now you can go back
to your own world" sets after the tournament (save flags are a 256-byte
array at `0x801D0618`, tested by `func_8002CCA8`).

**The mini-record family at WA 7475 is the per-duelist block**, indexed by
the opponent id (`D_8009B361`, 1-based; block 0 is a copy of block 1; ids
8 and 35 — Heishin's two duels — share their drop pools but not their
decks), 3 sectors each: deck
weights at `+0`, the S/A-POW, B/C/D and S/A-TEC drop pools at `+0x5B4`,
`+0xB68`, `+0x111C` (722 × u16 each, every one summing to 2048), the
rank-score table at `+0x16D0`. Ids 1–38 use 7475–7592 and block 39
(7592–7595) is Duel Master K: Villager 3's drop pools with a placeholder
deck, since his script plays a copy of the player's — all 40 × 4 weight
tables sum to 2048.

**The `0x80168000` packages, by screen.** WA 7968 (in the main-menu blob
at 7903) and 8054 (in the password-screen blob at 7983) carry the same
0x7800-byte **password-shop overlay** (identical but for the last data
sector); three GameShark patch codes verify
in it (`0x8016A87C` `subu $v1, $v1, $s0`, the star-cost subtraction;
`0x8016A880` `bnez`; `0x8016A6E0` `beqz`), and it tests/sets the per-card
"password used" flag `0x400 + card` at `0x8016A6D8`/`0x8016A764`. WA 7898
(end of the Free Duel blob at 7816) is the **Free Duel overlay**: its
unlock loop at `0x801683C0`–`0x801683EC` marks 40 grid entries and clears,
for ids 1–38, those whose save flag `0x6E0 + id` is off; the "all opponents"
patch code verifies on its `bnez` at `0x801683D4`. Four of the five function
addresses Unchiga read from a live disassembly of that screen land on
`addiu $sp` prologues in this image.

**WA 7767 (49 sectors) is the campaign's scene loader** (`func_8002FD10`):
its `0x1000` phase to `0x801A8000` is the campaign event script — a
`u16 offset[199]` table and 199 byte-coded events run through the 23-entry
table at `0x80090C50`. WA 7629 (138) is the Library (`func_8002BFCC`);
WA 7903–8069 are the main menu, Free Duel, password and shop screens.
