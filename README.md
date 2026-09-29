# Yu-Gi-Oh! Forbidden Memories Decompilation

[![Matching build](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml/badge.svg)](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml)

This repository is a byte-matching decompilation of the North American
(`SLUS-01411`), Japanese (`SLPM-86398`), European (`SLES-03947`), French
(`SLES-03948`), German (`SLES-03949`), Italian (`SLES-03950`), and Spanish
(`SLES-03951`) PlayStation releases of
**Yu-Gi-Oh! Forbidden Memories**. Accepted changes must continue to rebuild
each supported PS-X executable exactly.

The French target's shared-C matches, input requirements, and clean
build command are documented in [French matching](notes/french-matching.md).
The generated snapshot includes its configured overlay inventories but does
not yet report French resident metrics.

> [!IMPORTANT]
> The repository does not contain game data or proprietary Psy-Q tools. Supply
> legally obtained copies of the required files beneath `game/`; they remain
> ignored by Git.

## Project status

The generated report covers configured resident and overlay inventories, not
an exhaustive census of runtime code. A 100% resident row does not establish
complete overlay coverage. The newly identified
[duel-effect and boot banks](notes/overlays/duel-effect-bank.md) also exist in
Spanish, Italian, and German; they are not yet fully represented in those
regions' overlay tables, and their decompilation remains in progress.

<!-- BEGIN GENERATED PROGRESS -->

### North American (`SLUS-01411`)

Target SHA-256: `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,134 / 1,134 (100.00%)** |
| Game C-decompilation target bytes matched | **356,080 (`0x56EF0`) / 356,080 (`0x56EF0`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,195 |
| Preserved Psy-Q CRT/SDK assembly | 591 functions, 117,348 (`0x1CA64`) |
| Total discovered functions | 1,786 |
| Embedded/unassigned resident text | 1,780 (`0x6F4`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 53 / 85 (62.35%) | 18,396 (`0x47DC`) / 81,856 (`0x13FC0`) (22.47%) |
| `free_duel` | 9 / 9 (100.00%) | 4,140 (`0x102C`) / 4,140 (`0x102C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 17,724 (`0x453C`) / 17,724 (`0x453C`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password` | 27 / 27 (100.00%) | 10,884 (`0x2A84`) / 10,884 (`0x2A84`) (100.00%) |

_Generated from `config/slus_01411/functions.csv` and `config/slus_01411/overlays/*_functions.csv` by `tools/project/progress.py`._

### Japanese (`SLPM-86398`)

Target SHA-256: `ee3f45584fb747fd33c9560f0fc68ced03b399fbd9a2e9d6a71eb0f5daa89585`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,135 / 1,135 (100.00%)** |
| Game C-decompilation target bytes matched | **351,252 (`0x55C14`) / 351,252 (`0x55C14`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,196 |
| Preserved Psy-Q CRT/SDK assembly | 630 functions, 122,064 (`0x1DCD0`) |
| Total discovered functions | 1,826 |
| Embedded/unassigned resident text | 1,832 (`0x728`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 57 / 85 (67.06%) | 21,284 (`0x5324`) / 81,856 (`0x13FC0`) (26.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,140 (`0x102C`) / 4,140 (`0x102C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 17,724 (`0x453C`) / 17,724 (`0x453C`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password` | 27 / 27 (100.00%) | 10,280 (`0x2828`) / 10,280 (`0x2828`) (100.00%) |

_Generated from `config/slpm_86398/functions.csv` and `config/slpm_86398/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### European (`SLES-03947`)

Target SHA-256: `49544302dbe341489ae0eaf7888cac002a6160c57454970c434cdcb171d2a40e`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,184 (`0x57340`) / 357,184 (`0x57340`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 62 / 85 (72.94%) | 23,480 (`0x5BB8`) / 81,804 (`0x13F8C`) (28.70%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03947/functions.csv` and `config/sles_03947/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### Spanish (`SLES-03951`)

Target SHA-256: `b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 65 / 85 (76.47%) | 27,148 (`0x6A0C`) / 81,804 (`0x13F8C`) (33.19%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03951/functions.csv` and `config/sles_03951/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### French (`SLES-03948`)

Resident progress is not included here; the following counts cover only the inventoried runtime overlays.

Source: `config/sles_03948/overlays/*_functions.csv`, validated against their matching-C manifests.

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 63 / 85 (74.12%) | 22,944 (`0x59A0`) / 81,804 (`0x13F8C`) (28.05%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |


### Italian (`SLES-03950`)

Target SHA-256: `a01cc55d48df37c6bf9c6e2dfcbb4f429b4f8f2b111e35b44930d56bb12f9b1d`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03950/functions.csv` and `config/sles_03950/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### German (`SLES-03949`)

Target SHA-256: `d9ba940664c6f2c908b3f10d9a8232b0ea830155150a3ff234a2ac12ea3f07cc`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03949/functions.csv` and `config/sles_03949/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

<!-- END GENERATED PROGRESS -->

The resident progress tables separate game-owned C-decompilation targets from
evidence-backed handwritten assembly and preserved Psy-Q CRT/SDK routines.
Each version has its own authoritative resident function inventory, validated
against its matching manifest. Runtime-overlay inventories are reported
separately for all supported regions, including both password variants where
present.
Overlay percentages describe identified function boundaries, not all executable
bytes: the Japanese, European, French, and Spanish overworld modules include
preserved code outside their inventoried function boundaries, excluded from their
denominators. See
[function-inventories.md](notes/function-inventories.md) for the reusable
version-neutral inventory scaffold.

Run `make progress` when intentionally refreshing the project-wide snapshot.
It updates the generated table above and writes detailed machine-readable
metrics to `tmp/reports/progress.json`. Routine decompilation changes do not
need to update or commit the snapshot; `make check-progress` remains available
as an opt-in consistency check.

## Quick start

Place the original executables at `game/SLUS_014.11` and
`game/japanese/SLPM_863.98`, then run:

```sh
make verify-target
make verify-japanese-target
make tools
MAKEFLAGS=-j"$(nproc)" make match
MAKEFLAGS=-j"$(nproc)" make japanese-match
```

The match targets succeed only when each rebuilt executable is byte-identical
to its retail target. Between clean acceptance builds, seed the object cache
immediately after an unchanged matching build, then use the incremental edit
loop:

```sh
tools/environments/python/bin/python tools/project/build_incremental.py --seed-existing
MAKEFLAGS=-j"$(nproc)" make match-incremental
```

Warm incremental builds reuse content-validated split output and unchanged
objects, but still relink and compare the entire executable. The first split
cache population regenerates once. Finish accepted changes with `make match`;
see [incremental build details](notes/build.md#incremental-edit-builds).

The full repository audit additionally requires the DATA files and BIN/CUE
listed in `config/slus_01411/files.sha256`:

```sh
MAKEFLAGS=-j"$(nproc)" make audit
```

Those DATA files do not have to be sourced separately. With the retail disc at
`game/rpg-yfm.cue` and `game/rpg-yfm.bin`, `make disc-files` extracts every one
of them straight out of the image at the LBAs recorded in
`config/slus_01411/disc_layout.json`:

```sh
make disc-files                          # every tracked DATA file
make disc-files FILES="WA_MRG.MRG SU.MRG"  # only the overlay archives
```

Extraction refuses to run unless the image matches the tracked `bin_sha256`,
and each extracted file is checked against its tracked SHA-256 before it
replaces anything on disk, so a wrong dump fails immediately instead of
surfacing later as a build mismatch. Files already present and correct are left
alone, so the target is safe to re-run.

The `nproc` examples allow Make to schedule independent prerequisites on all
logical CPUs. The incremental Python driver also uses a numeric `MAKEFLAGS`
job count to build invalidated components concurrently. Clean baseline object
construction and linking remain sequential. Set a smaller `-j` value
explicitly on memory-constrained systems.

## Decompilation workflow

1. Select a game-owned assembly function from
   `config/slus_01411/functions.csv`.
2. Explore materially distinct source structures, declarations, and compiler
   profiles as deeply as the function requires. Preserve precise mismatch
   evidence and candidates under `tmp/`; the historical six-row ledgers do
   not cap further research.
3. Accept C only when `make match` reproduces the entire executable exactly.
4. Commit the matching source and authoritative metadata. Refresh the README
   separately with `make progress` when a project-wide snapshot is desired.

Unmatched functions remain exact assembly fallbacks. Handwritten and Psy-Q
assembly are tracked separately from compiler-generated game code.

## Repository layout

| Path | Purpose |
|---|---|
| `src/game/` | Matching C for the resident executable |
| `src/overlays/` | Module-scoped runtime overlay sources and layout policy |
| `src/types.h` | Shared fixed-width primitive aliases |
| `asm/` | Exact assembly fallbacks and data assembly |
| `config/slus_01411/` | Function inventory, compiler profiles, symbols, and target metadata |
| `tools/` | Project scripts, pinned tools, and local toolchains |
| `notes/` | Research, plans, evidence, and detailed documentation |
| `tmp/` | Generated builds, reports, caches, and scratch work |
| `game/` | Ignored, immutable user-supplied retail inputs |

All commands must run from the repository root, and project work must remain
inside this working directory.

## Documentation

- [Setup and required inputs](notes/setup.md)
- [Build and exact-match workflow](notes/build.md)
- [Compiler and toolchain fingerprint](notes/toolchain.md)
- [Psy-Q runtime and SDK integration](notes/psyq.md)
- [Random-number generation](notes/rng.md)
- [Runtime overlay research and source layout](notes/overlays/README.md)
- [Decompilation plan](notes/decompilation-plan.md)
- [Completed remaining-function campaign](notes/remaining-decompilation-pass.md)
- [Semantic naming pass](notes/semantic-naming-pass.md)
- [Global usage data](notes/global-usage.csv)
