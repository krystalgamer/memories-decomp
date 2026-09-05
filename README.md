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
| Matching C functions | **875 / 1,196 (73.16%)** |
| Matching C bytes | **115,812 (`0x1C464`) / 396,212 (`0x60BB4`) (29.23%)** |
| Remaining compiler-generated game assembly | 258 functions, 234,164 (`0x392B4`) |
| Intentional handwritten game assembly | 63 functions, 46,236 (`0xB49C`) |
| Preserved Psy-Q CRT/SDK assembly | 598 functions, 117,332 (`0x1CA54`) |
| Total discovered functions | 1,794 |
| Embedded/unassigned resident text | 1,780 (`0x6F4`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 5 / 9 (55.56%) | 672 (`0x2A0`) / 4,140 (`0x102C`) (16.23%) |
| `main_menu` | 17 / 31 (54.84%) | 3,248 (`0xCB0`) / 17,724 (`0x453C`) (18.33%) |
| `overworld_after_coup` | 10 / 14 (71.43%) | 2,544 (`0x9F0`) / 6,184 (`0x1828`) (41.14%) |
| `overworld_before_coup` | 10 / 14 (71.43%) | 2,544 (`0x9F0`) / 6,184 (`0x1828`) (41.14%) |
| `password` | 19 / 27 (70.37%) | 3,244 (`0xCAC`) / 10,884 (`0x2A84`) (29.81%) |

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
- [Psy-Q runtime and SDK integration](notes/psyq.md)
- [Random-number generation](notes/rng.md)
- [Runtime overlay research and source layout](notes/overlays/README.md)
- [Decompilation plan](notes/decompilation-plan.md)
- [Completed remaining-function campaign](notes/remaining-decompilation-pass.md)
- [Semantic naming pass](notes/semantic-naming-pass.md)
- [Global usage map](notes/global-usage.md)
