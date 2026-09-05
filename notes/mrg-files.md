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
