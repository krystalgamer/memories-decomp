# Yu-Gi-Oh! Forbidden Memories Decompilation

[![Matching build](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml/badge.svg)](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml)

This repository is a byte-matching decompilation of the North American
PlayStation release of **Yu-Gi-Oh! Forbidden Memories** (`SLUS-01411`).
Accepted changes must continue to rebuild the complete PS-X executable exactly.

> [!IMPORTANT]
> The repository does not contain game data or proprietary Psy-Q tools. Supply
> legally obtained copies of the required files beneath `game/`; they remain
> ignored by Git.

## Project status

The current mixed C/assembly build reproduces `game/SLUS_014.11` with SHA-256:

```text
84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

<!-- BEGIN GENERATED PROGRESS -->

| Metric | Current |
|---|---:|
| Matching C functions | **914 / 1,196 (76.42%)** |
| Matching C bytes | **131,500 (`0x201AC`) / 396,212 (`0x60BB4`) (33.19%)** |
| Remaining compiler-generated game assembly | 219 functions, 218,476 (`0x3556C`) |
| Intentional handwritten game assembly | 63 functions, 46,236 (`0xB49C`) |
| Preserved Psy-Q CRT/SDK assembly | 598 functions, 117,332 (`0x1CA54`) |
| Total discovered functions | 1,794 |
| Embedded/unassigned resident text | 1,780 (`0x6F4`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 6 / 9 (66.67%) | 1,496 (`0x5D8`) / 4,140 (`0x102C`) (36.14%) |
| `main_menu` | 25 / 31 (80.65%) | 7,564 (`0x1D8C`) / 17,724 (`0x453C`) (42.68%) |
| `overworld_after_coup` | 12 / 14 (85.71%) | 3,648 (`0xE40`) / 6,184 (`0x1828`) (58.99%) |
| `overworld_before_coup` | 12 / 14 (85.71%) | 3,648 (`0xE40`) / 6,184 (`0x1828`) (58.99%) |
| `password` | 20 / 27 (74.07%) | 3,916 (`0xF4C`) / 10,884 (`0x2A84`) (35.98%) |

_Generated from `config/slus_01411/functions.csv` and `config/slus_01411/overlays/*_functions.csv` by `tools/project/progress.py`._

<!-- END GENERATED PROGRESS -->

Matching progress covers game-owned code only. Psy-Q CRT/SDK functions are
identified and preserved as assembly rather than counted as decompilation
targets.

Run `make progress` when intentionally refreshing the project-wide snapshot.
It updates the generated table above and writes detailed machine-readable
metrics to `tmp/reports/progress.json`. Routine decompilation changes do not
need to update or commit the snapshot; `make check-progress` remains available
as an opt-in consistency check.

## Quick start

Place the original executable at `game/SLUS_014.11`, then run:

```sh
make verify-target
make tools
MAKEFLAGS=-j"$(nproc)" make match
```

`make match` succeeds only when the rebuilt executable is byte-identical to the
retail target. The full repository audit additionally requires the DATA files
and BIN/CUE listed in `config/slus_01411/files.sha256`:

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

The examples use all logical CPUs reported by `nproc`. Set a smaller `-j`
value explicitly on memory-constrained systems.

## Decompilation workflow

1. Select a game-owned assembly function from
   `config/slus_01411/functions.csv`.
2. Record each distinct source/compiler attempt in the appropriate attempt
   ledger, with a maximum of six variants per function.
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
