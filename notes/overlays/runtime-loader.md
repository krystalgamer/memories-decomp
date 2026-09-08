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

A pre-title-screen runtime trace observed all seven entries initialized to
these values and the following filename pointer set to null. The populated
table therefore matches the independently measured retail disc extents.

`File_SetPositionTable` enumerates these names. `File_GetPosition` repeatedly
performs `DsSearchFile` and converts BCD MSF to a zero-based LBA with
`CdPosToInt_8007E710`.

The BSS layout is:

```text
0x800E9E18-0x800E9E5F  queued secondary transfer descriptor, 0x48 bytes
0x800E9E60-0x800E9EA7  active primary transfer descriptor, 0x48 bytes
0x800E9EA8-0x800E9EC3  seven runtime LBA words
```

`File_ActivateTransfer` copies the complete secondary descriptor into the
primary descriptor before publishing the primary-active state.

Its raw `Block72` assignment retains `FILE_TRANSFER_DESCRIPTOR_WORD_COUNT`
(`18`) signed 32-bit words, including the final substate byte. Size assertions
tie that copy and the private initializer view to the shared `0x48`-byte
descriptor without changing their types or copy operations. The separate
eight-word/32-byte buffer record at `D_801D4200` is not a descriptor and keeps
its existing independent copy.

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

The shared `FileTransferDescriptor` explicitly exposes `substate` at `+0x47`;
it is not unused tail padding. The private initializer clears that byte, and
the completion alias `D_800E9EA7[0]` addresses the same byte of the primary
descriptor. The existing partial field names and local views remain intact.

Type-free constants in `file_constants.h` name the status-flags, state, and
substate byte offsets (`+0x2C`, `+0x46`, and `+0x47`), with assertions against
the shared descriptor. Its legacy `done` member is the transfer-state byte,
not a Boolean completion flag. Raw users keep their signed flags-word read,
byte accesses, and local padding views; no state values or branch behavior
are changed by naming the offsets.

`func_80013940` interprets its third argument (`position`) and fourth argument
(`size`) by sign. The matching body in `src/game/file_stream.c` applies:

```c
file_index = file_flags & FILE_TRANSFER_FILE_INDEX_MASK;
descriptor->total_bytes = size;
if (size < 0)
    descriptor->total_bytes = -(size << 11);

if (position < 0) {
    descriptor->file_bytes = 0;
    descriptor->absolute_lba = -position;
} else {
    descriptor->file_bytes = position << 11;
    descriptor->absolute_lba = gFile_anLba[file_index] + position;
}
```

`FILE_TRANSFER_FILE_INDEX_MASK` (`0xF`) extracts the selector from the low
nibble; it is not a bounds check against `FILE_POSITION_TABLE_CAPACITY` (`7`).
The existing masking points remain in place, selectors `7..15` are not
clamped, and the separate direct-index request path is unchanged.

Thus a nonnegative position is a file-relative sector offset; a negative
position supplies the negated absolute LBA and bypasses the file LBA table.
The retail branches at `0x80013944` and `0x80013958` independently select the
size and position modes. In the helper, nonnegative sizes are already bytes;
only negative sizes are converted from sectors.

`File_InitTransferDescriptor` negates its fifth argument before passing it as
the helper's fourth argument (`negu a3,v0` at `0x800139D8`, in the call's delay
slot). Its public size convention is therefore the reverse:

| Initializer size argument | Helper size argument | Descriptor total bytes |
|---:|---:|---:|
| `3` | `-3` | `0x1800` (three sectors) |
| `-513` | `513` | `513` (bytes, with no sector rounding) |
| `0` | `0` | `0` |

The initializer's default image rectangle is
`FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH` (`0x40`) 16-bit VRAM words wide by
`FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT` (`0x10`) rows. Its payload is exactly
one `FILE_SECTOR_SIZE`: `64 * 16 * 2 = 2048` bytes, enforced by a compile-time
size relation. These are VRAM transfer units, not a claim about texture texel
width at every bit depth. The existing image-path conditions, packed
coordinates, and callback-specific rectangle overrides remain unchanged.

`File_RequestAsyncTransfer` is the common game-facing asynchronous loader
using the active descriptor. It forwards its third and fourth arguments as
the initializer's position and size, respectively: nonnegative positions are
file-relative sectors, negative positions are negated absolute LBAs, positive
sizes count sectors, and negative sizes give the negated byte count. For
example, position `-954` and size `-513` select absolute LBA `954`, file byte
offset zero, and 513 total bytes. Its other arguments provide the file
selector, caller-specific loader argument, phase callback, phase seed, and
optional direct destination.

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
| Boot UI package | `5776-5830` | 54 | `0xB48000-0xB63000` |
| Seven duel-terrain packages | `5830-7475` | `7 * 235` | `0xB63000-0xE99800` |
| Opponent deck/drop/rank records | `7475-7595` | inferred `40 * 3` | `0xE99800-0xED5800` |
| Duel-results package | `7595-7629` | 34 | `0xED5800-0xEE6800` |
| Library package | `7629-7767` | 138 | `0xEE6800-0xF2B800` |
| Campaign-scene package | `7767-7816` | 49 | `0xF2B800-0xF44000` |
| Free Duel package | `7816-7903` | 87 | `0xF44000-0xF6F800` |
| Name-entry package | `7903-7983` | 80 | `0xF6F800-0xF97800` |
| Password-screen package | `7983-8069` | 86 | `0xF97800-0xFC2800` |
| Fixed package | `8069-8103` | 34 | `0xFC2800-0xFD3800` |
| Menu background/symbol package | `8103-8153` | 50 | `0xFD3800-0xFEC800` |
| Egypt overworld before coup | `8153-8311` | 158 | `0xFEC800-0x103B800` |
| Egypt overworld after coup | `8311-8469` | 158 | `0x103B800-0x108A800` |
| Fixed package | `8469-8519` | 50 | `0x108A800-0x10A3800` |
| Direct bank load | `8519-8535` | 16 | `0x10A3800-0x10AB800` |
| Fixed package | `8535-8585` | 50 | `0x10AB800-0x10C4800` |
| Fixed package | `8585-8661` | 76 | `0x10C4800-0x10EA800` |

The next loader family begins from sector 8661, reinforcing the interpretation
of these values as generated merge boundaries.

## Seven 235-sector WA records

Matching `func_8001798C` now expresses the request with the shared source
constants:

```text
record sector = DUEL_TERRAIN_PACKAGE_FIRST_SECTOR (0x16C6)
              + gDuel_bTerrain * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT (0xEB)
record count  = DUEL_TERRAIN_PACKAGE_SECTOR_COUNT (0xEB)
callback      = Duel_LoadPackageStage
```

The following independently indexed family begins at sector 7475:

```text
7475 - 5830 = 1645 = 7 * 235
```

This proves seven physical records. The accepted terrain domain identifies
indices zero through six as Normal, Forest, Wasteland, Mountain, Meadow, Sea,
and Dark; the disc-side comparison below independently confirms that mapping.

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

`func_80024E58` uses the second named range when a terrain effect reloads its
data:

```text
effect sector = DUEL_TERRAIN_EFFECT_DATA_FIRST_SECTOR (0x1791)
              + gDuel_bTerrain * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT (0xEB)
effect count  = DUEL_TERRAIN_EFFECT_DATA_SECTOR_COUNT (0x10)
```

`0x1791 - 0x16C6 = 0xCB`, so this request begins at relative sector 203,
exactly where phase 12 starts. Its 16 sectors (`0x8000` bytes) are the first
half of that final 32-sector (`0x10000`-byte) phase.

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

Verified WA phases directed to `0x80168000` include:

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

#### Startup phase and enable-code sites

Matching [`func_80043960`](../../src/game/func_80043960.c) requests WA
`[5776, 5830)`. Callback
[`func_80043328`](../../src/game/func_80043328.c) assigns consecutive phase
sizes `0x18000`, `0x1000`, `0x800`, and `0x1800`; the last phase goes to
the pointer at `D_800101D8`. The original executable stores `0x80168000`
there. Thus the final image is WA `[5827, 5830)`, file bytes
`[0xB61800, 0xB63000)`, mapped to RAM `[0x80168000, 0x80169800)`.

The image's leading word is `0x00000016` and its SHA-256 is
`83d49e3fde2dca5e60961ac9bcf31fd1ce918c885f6b88fcab01496691581d3a`.
It is a verified load phase, but not one of the five configured images in
`config/slus_01411/overlays.json`.

Both published "enable" GameShark guards match this phase:

| Guarded RAM address | WA word offset | Original word | Branch target |
|---|---:|---:|---:|
| `0x80168188` | `0xB61988` | `0x1040023A` | `0x80168A74` |
| `0x80168100` | `0xB61900` | `0x1062000A` | `0x8016812C` |

Writing `0x1000` to either instruction's high halfword makes its existing
branch unconditional without changing the destination. The second write is
the `0xB61902` archive alteration discussed in the
[patched-dump comparison](README.md#the-anti-piracy-patched-dump-does-not-affect-these-modules).
The [GameShark evidence](../research/gameshark-codes.md#located-enable-code-guards)
also records the nonmatching configured-image controls. These are static
load/word identifications, not a claim that the patches were run or that
their complete gameplay effects are known.

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
the seven-record phase layout and terrain identity, the two Egypt overworld
variants, archive-to-slot attribution, and code at exact resident call targets.

The following remain provisional:

- Whether the leading words are formally named module IDs.
- The original source/header name for the WA destination table.
- Original unrounded inner-file sizes.
- Full MODEL and SU merged-file manifests.
- Reads that might be issued by dynamically loaded code rather than resident
  code.

## Semantic identities, from the disc side

Measured in the `MaChInEgUn3/ygofm-decomp` tree against the same NTSC-U
disc; the layout above stands as written, this only fills some of its
provisional items.

**The seven 235-sector records are the seven terrain types.** The index in
`func_8001798C` is the terrain byte `gDuel_bTerrain` (0 normal, 1 forest, 2
wasteland, 3 mountain, 4 meadow, 5 sea, 6 dark). Hashing all thirteen phases
across the seven records, twelve are byte-identical and only phase 12 (the
last `0x10000`, VRAM (640, 256)) differs — the field picture. So the seven
copies are seven backgrounds each shipped with a duplicate of the same 416
KB.

### Shared duel-hand graphics block

The community tutorial `Fields WA.txt` lists one "DUEL DISK HAND" offset for
each terrain and one for the Library. The seven duel offsets follow the
recovered record geometry exactly:

| Context | Package start | Tutorial offset | Relative offset |
|---|---:|---:|---:|
| Normal | `0xB63000` | `0xB7B000` | `+0x18000` |
| Forest | `0xBD8800` | `0xBF0800` | `+0x18000` |
| Wasteland | `0xC4E000` | `0xC66000` | `+0x18000` |
| Mountain | `0xCC3800` | `0xCDB800` | `+0x18000` |
| Meadow | `0xD39000` | `0xD51000` | `+0x18000` |
| Umi | `0xDAE800` | `0xDC6800` | `+0x18000` |
| Yami | `0xE24000` | `0xE3C000` | `+0x18000` |
| Library | `0xEE6800` | `0xEFE800` | `+0x18000` |

For each terrain record, this is the final `0x8000` bytes (16 sectors) of
phase 0. The Library copy begins at the same package-relative offset. All
eight `0x8000`-byte ranges are byte-identical:

```text
SHA-256: 5e94e1a373c529a29e03cf1845eea74c1ac29a94f607a00d479df1cd4c9e1f13
```

This confirms that the tutorial offsets select duplicate copies of one shared
graphics payload rather than terrain-specific art. Editing one offset changes
only the package that contains that copy; applying the same replacement to
every duel terrain and the Library requires patching all eight ranges. The
tutorial's visual identification as the duel-hand graphic is high confidence
from observed use, while the archive placement and byte identity are confirmed
directly from the retail `WA_MRG.MRG`.

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
array at `0x801D0618`, tested by `Campaign_TestStoryFlag`).

**The mini-record family at WA 7475 is the per-duelist block**, indexed by
the opponent id (`gDuel_bOpponentID`, 1-based; block 0 is a copy of block 1; ids
8 and 35 — Heishin's two duels — share their drop pools but not their
decks), 3 sectors each: deck
weights at `+0`, the S/A-POW, B/C/D and S/A-TEC drop pools at `+0x5B4`,
`+0xB68`, `+0x111C` (722 × u16 each, every one summing to 2048), the
rank-score table at `+0x16D0`. Ids 1–38 use 7475–7592 and block 39
(7592–7595) is Duel Master K: Villager 3's drop pools with a placeholder
deck, since his script plays a copy of the player's — all 40 × 4 weight
tables sum to 2048.

**The `0x80168000` packages, by screen.** WA 7968 (in the name-entry blob
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
table at `0x80090C50`.

### Library package phases

`func_8002BFCC` requests 138 WA sectors beginning at sector `0x1DCD`.
Resident callback `func_8002BD0C` has six transfer phases followed by one
finalization mode:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0xEE6800-0xF06800` | `0x20000` / 64 sectors | Schedules the first image payload through the GPU/VRAM transfer path. Its last `0x8000` bytes are the byte-identical Library copy of the duel-hand graphics. |
| `0xF06800-0xF08800` | `0x2000` / 4 sectors | Stages palette data, then uploads the complete block as a `256 x 16` rectangle to VRAM `(256, 240)`. |
| `0xF08800-0xF20800` | `0x18000` / 48 sectors | Schedules the second image payload through the GPU/VRAM transfer path. |
| `0xF20800-0xF21000` | `0x800` / 1 sector | Stages a second palette block. The callback uploads its first `0x400` bytes as a `256 x 2` rectangle to VRAM `(256, 246)`; the remaining `0x400` bytes are zero padding. |
| `0xF21000-0xF2B000` | `0xA000` / 20 sectors | Uses transfer state `3`, the alternating sector buffer, and descriptor value `0x26810`; its destination and later role remain unnamed. |
| `0xF2B000-0xF2B800` | `0x800` / 1 sector | Stages one final sector, then calls `func_80048D08(1, buffer)`; the resident routine's package-specific role remains unnamed. |

The six sizes total the requested `0x45000` bytes exactly. No phase is copied
directly to a callable `0x80168xxx` module slot, so the resident loader
provides no evidence for a separate Library overlay.

WA 7903–8069 are the name entry, Free Duel, password and shop screens.

**Screen packages are named by their resident request.** Each front-end
screen asks for its package through the same helper, whose third and fourth
arguments are the first WA sector and the sector count:

```text
File_RequestAsyncTransfer(
    0, 0, <first sector>, <sector count>, <screen callback>, 0, 0
)
```

| Screen | Requesting function | Arguments | WA package |
|---|---|---|---|
| Free Duel | `Main_InitFreeDuelMenu` | `0x1E88`, `0x57` | `7816-7903` |
| Name entry | `func_8003BBF8` | `0x1EDF`, `0x50` | `7903-7983` |
| Password | `func_8003BEB8` | `0x1F2F`, `0x56` | `7983-8069` |
| Egypt overworld | `func_8003C0C0` | `0x1FD9`, `0x9E`; `+0x9E` on story flag `0x47` | `8153-8311`, `8311-8469` |

The package at `7903` is therefore the **name entry** screen, not the main
menu. The main menu is not a WA package at all: `func_8005B85C` requests
`File_RequestAsyncTransfer(1, gFile_szSuMrgPath, 0, 0x73, func_8005B64C, 0,
0)`, where `gFile_szSuMrgPath` at `0x800117C8` is the path literal
`M:/mrgSU/SU.mrg`, and its executable phase is SU sectors `98-114` loaded at
`0x80180000`.
