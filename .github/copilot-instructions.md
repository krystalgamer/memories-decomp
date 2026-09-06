# Copilot instructions for memories-decomp

## Build, validation, and repository checks

Run every project command from the repository root. Project scripts reject a
different working directory and paths that escape the repository.

Keep all work inside this repository root. Put temporary sources, probes,
build artifacts, logs, diffs, and other scratch work under `tmp/`; do not use
paths elsewhere on the machine. User-provided reference material for
decompilation and naming work is placed under `tmp/references/`.

The build requires a legally obtained North American executable at
`game/SLUS_014.11`. Full disc checks also require every ignored input listed in
`config/slus_01411/files.sha256`.

```sh
# First-time local setup
make verify-target
make tools
make check-tools

# Clean exact-match gate used by CI
make clean
MAKEFLAGS=-j"$(nproc)" make match

# Full local repository audit; requires all retail inputs and a clean worktree
MAKEFLAGS=-j"$(nproc)" make audit
```

There is no unit-test runner or conventional lint target. Validation is based
on reproducing the complete executable byte-for-byte plus repository policy
checks:

```sh
make basic-types          # every C file uses src/types.h
make external-attempts    # validate external/refinement attempt records
make check-global-usage   # optional generated global-usage consistency check
make check-progress       # optional README progress consistency check
```

For repeated work on one function or translation unit, seed the incremental
object cache after a clean match, then use the incremental exact-match target.
It rebuilds changed objects but still relinks and hashes the entire executable:

```sh
MAKEFLAGS=-j"$(nproc)" make match
tools/environments/python/bin/python \
  tools/project/build_incremental.py --seed-existing

# After editing one C source
MAKEFLAGS=-j"$(nproc)" make match-incremental

# Final acceptance must still use the clean path
MAKEFLAGS=-j"$(nproc)" make match
```

Use `MAKEFLAGS=-j"$(nproc)"` so Make follows the host's available logical
CPUs. Set a lower job count explicitly on memory-constrained systems.
Matching, attempt-ledger updates, and integration remain sequential.

## Architecture

- `game/` contains ignored, immutable retail inputs. Nothing from this
  directory may be committed.
- `config/slus_01411/functions.csv` is the authoritative resident-function
  inventory: address, size, name, status, ownership, and durable notes.
  `matching_c.json` maps each matching function to its source and named
  compiler profile; `compiler_profiles.json` defines the exact GCC/MASPSX
  pipeline. Symbol, relocation, image-map, and disc metadata live beside them.
- `src/game/` contains tracked matching C. A source may define one function or
  a contiguous group of functions. `src/types.h` is the sole definition point
  for the fixed-width primitive aliases.
- `make split` runs `tools/project/generate_build_config.py`, which combines
  the static `config/slus_01411/split.yaml` template with `matching_c.json`.
  Splat then writes generated assembly, data, assets, symbols, and diagnostics
  beneath `tmp/splat/`; generated build manifests go beneath `tmp/generated/`.
  These generated files are not source and must not be edited or committed.
- `tools/project/build_baseline.py` assembles unmatched generated MIPS,
  compiles matching C with its per-function profile, normalizes compiler
  assembly through the matching MASPSX version, converts binary regions to
  objects, and links everything in executable order.
- `config/slus_01411/split.yaml` fixes the original image layout and directs
  Splat to generate `tmp/splat/slus_01411.ld`;
  `config/slus_01411/link_symbols.ld` supplies the extra layout symbols that
  generated script does not define.
  `tools/project/build_baseline.py` links through those generated/tracked
  scripts, and `tools/project/match.py` accepts the build only when
  `tmp/project-build/SLUS_014.11` has the retail target hash.
- `notes/` holds durable research and workflow documentation. Progress and
  global-usage data are generated from tracked metadata, not maintained as
  independent sources of truth.
- `tmp/references/` contains user-provided comparison sources, exports, and
  other evidence used during decompilation. Treat references as evidence
  rather than authoritative declarations or build configuration, and keep all
  derived reference work under `tmp/`.

## Matching and metadata conventions

- Functional equivalence is insufficient. Preserve instruction bytes,
  relocations, object order, section placement, executable size, and final
  SHA-256. Source that looks unusual may intentionally control old-GCC
  register allocation or scheduling; do not simplify it without an exact
  match.
- Keep unmatched code as the generated assembly fallback until a C candidate
  passes full-executable matching. Only game-owned functions are decompilation
  candidates; Psy-Q CRT/SDK functions never belong in the attempt ledgers.
- Use named profiles from `compiler_profiles.json`; do not hide one-off flags
  in ad hoc commands. Try GCC 2.8.1 profiles first. Use GCC 2.7.2 only after
  recorded evidence indicates that cohort or the useful 2.8.1 profiles have
  been explored.
  Pair GCC 2.8.1 with MASPSX 2.81 and GCC 2.7.2 with MASPSX 2.72.
- Record every materially distinct source/compiler experiment and its precise
  mismatch reason. The original six-row canonical ledgers are historical
  campaign evidence, not a limit on further investigation; keep additional
  candidates, compiler probes, and diffs under `tmp/` until an exact result is
  ready for durable integration.
- Keep candidate sources, compiler probes, object files, and diffs under
  `tmp/`. After a terminal matched record, promote the verified source with
  `tools/project/integrate_verified_match.py`; it updates
  `functions.csv` and `matching_c.json`. Do not hand-edit generated Splat or
  `tmp/generated/` manifests.
- Multiple `matching_c.json` entries may share one source only when the
  functions are contiguous, use the same compiler profile, appear in source
  definition order, and exactly cover the grouped object. Do not split,
  reorder, or partially replace a grouped translation unit without preserving
  those invariants and the full executable match.
- Treat `make progress` and `make global-usage` as optional project-wide
  report updates, not part of each decompilation change. Matching PRs do not
  need to refresh either output.

## C and naming conventions

- Every C source must include the relative path to `src/types.h` and use
  `s8/u8`, `s16/u16`, `s32/u32`, and `s64/u64`; do not redeclare these aliases.
  Keep uncertain structs and callback types local until repeated offsets and
  ownership justify a shared header.
- Use address-based names such as `func_80012345` and `D_80012345` until
  semantics are supported by callers, data layout, strings, SDK signatures, or
  observed behavior. Do not rename symbols merely to reduce the unknown count.
- Evidence-backed function names use `<Subsystem>_<Verb><Object>`; named
  globals generally use `g<Subsystem>_<Role>`. Preserve accepted sound-driver
  forms such as `SD_...` and `g_SDValue`.
- Record semantic renames with the original address, evidence, confidence, and
  caveats in `notes/semantic-symbol-map.csv`. Only confirmed or high-confidence
  names replace address-based symbols.
- Do not track retail files, generated `tmp/` output, downloaded tools,
  installed environments, vendor checkouts, or local toolchains. Durable
  Markdown belongs under `notes/`, except for per-directory `README.md` files
  and this instruction file.
- Commits you author must use the `Copilot` name with a
  `+Copilot@users.noreply.github.com` e-mail address, and may only carry a
  `Co-authored-by` trailer for that same identity. Commits from other authors
  are kept as-is and are not held to those rules.
