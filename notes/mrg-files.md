# MRG File Model

## Confirmed container model

The project does not treat `*.MRG` as self-describing archives. The original
build scripts merged input files by concatenation and generated offsets,
counts, or resource-specific loader code that was compiled into the game.

Consequences:

- Do not infer a header from the first words of an MRG.
- The authoritative index is executable code and data, not an archive
  directory.
- Inner boundaries must be recovered from seek/count arguments, callback phase
  tables, affine index calculations, and exact checks against the merged bytes.
- Different MRG files may use different generated layouts even though all were
  produced by concatenation.
- Repacking would require reproducing both the byte concatenation and the
  generated source tables or macros.

## Resident file indices

The filename table at `0x8009078C` and runtime LBA table at `0x800E9EA8`
establish:

| Index | File | Disc LBA |
|---:|---|---:|
| 0 | `WA_MRG.MRG` | 10,102 |
| 1 | `SU.MRG` | 954 |
| 2 | `MODEL.MRG` | 28,534 |
| 3 | `MOVIE.STR` | 202,734 |
| 4 | `SD_SE.DAT` | 2,193 |
| 5 | `SD_BGM.DAT` | 2,936 |
| 6 | `MASTER.XA` | 199,930 |

A boot-time PCSX-Redux trace observed all seven words populated before the
Konami screen, with these exact values and a null filename pointer after index
6. This confirms that the runtime lookup agrees with the retail disc layout.

Loader offsets and counts for the three MRG files are expressed in
`0x800`-byte logical sectors. `func_80013940` converts a file-relative sector
to both a byte offset and an absolute disc LBA.

`src/game/file_constants.h` names this unit as `FILE_SECTOR_SIZE` and its
byte-conversion shift as `FILE_SECTOR_SHIFT`. The descriptor setup and transfer
control paths use them for sector-to-byte conversion and the one-sector gap
between alternating buffer destinations. This is the logical payload size,
not the disc image's 2,352-byte raw sector size.

## Shared high-memory slots

The fixed addresses beginning at `0x8013A000` are a shared runtime layout, not
a WA-only destination list. Resident loader traces currently establish:

| Destination | Confirmed resident source | Role |
|---:|---|---|
| `0x8013A000` | `MODEL.MRG` | Executable/module slot; called at `+4` |
| `0x80146000` | `WA_MRG.MRG` | Repeated executable module |
| `0x80168000` | `WA_MRG.MRG` | Reused executable module slot |
| `0x8017A1D8` | `WA_MRG.MRG` | Repeated data subrange |
| `0x8017A000` | `MODEL.MRG` | Executable content at the slot base |
| `0x80180000` | `SU.MRG` | Reused executable module slot |

WA therefore supplies several overlays and data ranges, but not every
high-memory slot. The table at `0x80010000` is a shared game layout table.

## Recovered WA organization

The resident executable contains no conventional 32-bit WA directory.
Generated-looking WA boundaries are encoded as immediate sector numbers,
stride arithmetic, transfer counts, and phase callbacks.

A contiguous layout has been recovered from WA sectors `5776-8661`, or byte
range `0xB48000-0x10EA800`. It includes seven 235-sector records, a likely
40-entry family of three-sector records, and multiple fixed packages. The
seven large records use a 13-phase callback whose counts total exactly 235
sectors.

The complete loader trace, descriptor layout, sector map, phase table, archive
attribution, and called-address checks are recorded in
`notes/overlays/runtime-loader.md`.

### End-of-duel results package

Resident `func_80020F4C` requests 34 WA sectors beginning at sector `0x1DAB`,
which is archive range `0xED5800-0xEE6800`. Matching callback
`func_80020BE4` accounts for the complete package:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0xED5800-0xEE5800` | `0x10000` / 32 sectors | Schedules the main image payload. The tutorial evidence divides it into two `0x8000`-byte image halves at `0xED5800` and `0xEDD800`. |
| `0xEE5800-0xEE6000` | `0x800` / 1 sector | Uploads a `256 x 4` palette rectangle to VRAM `(0, 248)`. |
| `0xEE6000-0xEE6800` | `0x800` / 1 sector | Schedules a transfer to `0x801AF000`; its later role remains unnamed. |

The palette sector has SHA-256
`bc1bbeb55e28cef036ec35e90d232b070df8221d5d114b4116a9743498fe7776`.
Its 16-colour slots cover the result, rank, statistics, card, and player
win/lose graphics described in
[`modding-tutorial-evidence.md`](modding-tutorial-evidence.md). Those visual
labels come from the tutorial; the package boundaries, upload geometry, and
final destination come from the resident request and matching callback.

### Menu background and symbol package

Resident `func_8002F630` requests 50 WA sectors beginning at sector `0x1FA7`,
which is archive range `0xFD3800-0xFEC800`. Matching callback
`func_8002F4C0` divides the package into three transfer phases:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0xFD3800-0xFEB800` | `0x18000` / 48 sectors | Schedules the image payload. The tutorial evidence divides it into a `0x10000`-byte menu-background range and a `0x8000`-byte symbol range at `0xFE3800`. |
| `0xFEB800-0xFEC000` | `0x800` / 1 sector | Stages palette data; the callback uploads its first `0x400` bytes as a `256 x 2` rectangle to VRAM `(0, 244)`. The remaining half-sector is not assigned a role. |
| `0xFEC000-0xFEC800` | `0x800` / 1 sector | Schedules a transfer to `0x801AF000`; its later role remains unnamed. |

The tutorial places a 256-colour background palette at `0xFEB800` and a
16-colour symbol palette at `0xFEBA00`, which map to the first full VRAM row
and the first 16 entries of the second row. See
[`modding-tutorial-evidence.md`](modding-tutorial-evidence.md) for the
resource hashes and the visual-label confidence boundary.

### Boot UI package

Matching `func_80043960` requests 54 WA sectors beginning at sector `0x1690`,
which is archive range `0xB48000-0xB63000`. Its matching callback
`func_80043328` accounts for all four transfer phases:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0xB48000-0xB60000` | `0x18000` / 48 sectors | Schedules the main boot UI image payload. |
| `0xB60000-0xB61000` | `0x1000` / 2 sectors | Uploads the complete block as a `256 x 8` rectangle to VRAM `(512, 248)`. |
| `0xB61000-0xB61800` | `0x800` / 1 sector | Uploads its first `0x100` bytes as a `16 x 8` rectangle to VRAM `(640, 232)`; retail bytes `0xB61100-0xB617FF` are zero padding. |
| `0xB61800-0xB63000` | `0x1800` / 3 sectors | Schedules a transfer to the destination pointer stored at `D_800101D8`; its later role remains unnamed. |

Tutorial offsets place the shared card-pointer and fusion-number graphics
inside the first phase, their palettes inside the second, and eight
16-colour UI ramps at `0xB61000-0xB61100` in the third. See
[`modding-tutorial-evidence.md`](modding-tutorial-evidence.md) for the
resource-level offsets, hashes, and visual-label confidence.

### Password screen package

Matching `func_8003BEB8` requests 86 WA sectors beginning at sector `0x1F2F`,
which is archive range `0xF97800-0xFC2800`. Its matching callback
`func_8003BD14` accounts for the complete package:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0xF97800-0xFB7800` | `0x20000` / 64 sectors | Schedules the password-screen image payload. |
| `0xFB7800-0xFB9800` | `0x2000` / 4 sectors | Uploads the complete block as a `256 x 16` palette rectangle to VRAM `(256, 240)`. |
| `0xFB9800-0xFBB000` | `0x1800` / 3 sectors | Transfers to `0x801A8000`. The 723 eight-byte password/cost records occupy the first `0x1698` bytes; the remaining bytes retain no assigned role. |
| `0xFBB000-0xFC2800` | `0x7800` / 15 sectors | Transfers the executable phase used by the tracked password overlay. |

Tutorial offsets place the card, cursor, and frame images in the first phase
and their palette rows in the second. The executable phase and its exact hash
are tracked under [`src/overlays/password/`](../src/overlays/password/).
See [`modding-tutorial-evidence.md`](modding-tutorial-evidence.md) for the
resource-level offsets and visual-label confidence.

## Development-path evidence

The executable preserves paths including:

```text
M:/mrgSU/SU.mrg
M:/mrgSU/model.mrg
```

The three resident copies have distinct addresses and consumers:

| Address | Symbol | Text | Established use |
|---:|---|---|---|
| `0x800117C8` | `gFile_szSuMrgPath` | `M:/mrgSU/SU.mrg` | Matching `func_8005B85C` passes it to `File_RequestAsyncTransfer` with file selector `1`, sector `0`, and count `0x73`; this is the named SU request that supplies the main-menu package. |
| `0x80011580` | `gFile_szModelMrgPath` | `M:/mrgSU/model.mrg` | Referenced by `Model_LoadMonsterMerge` in the resident model-loader cohort. |
| `0x800114F8` | `D_800114F8` | `M:/mrgSU/SU.mrg` | A second live copy referenced by `Model_LoadMonsterMerge` and `func_800507D0`; its exact role remains unnamed. |

These strings support the merge-script model and may identify the original
source module that included generated offsets. The duplicate `SU.mrg` strings
cannot share one linker symbol: both addresses have live references, so each
must retain an address-specific identity until its own use is understood.

## Scope

The original BIN/CUE remains an immutable LBA and sector-layout reference.
Recovering MRG boundaries is performed only when needed to understand resident
code or runtime overlays. General asset extraction and repacking remain outside
the executable matching target.
