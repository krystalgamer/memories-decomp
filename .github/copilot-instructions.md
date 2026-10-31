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

Use `make match-incremental` for intermediate C/header edits in the worker loop.
Do not run `make split` or `make clean` between those iterations: warm builds
validate and reuse the split output as well as unchanged objects. Seed only
immediately after an unchanged clean match. Keep the final clean acceptance
above; incremental success does not replace it.

Keep performance probes local beneath `tmp/`; do not upload build or performance
artifacts. Prioritize worker edit/build iterations over heavyweight CI benchmarks.

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
  in ad hoc commands. GCC 2.8.1 paired with MASPSX 2.81 is the supported
  pipeline, so vary named profiles rather than compilers. The only exception
  is an evidence-backed one: the specific MODEL.MRG variant functions and
  images whose tracked manifests select the named `gcc_2_7_2_cdk_g0` profile
  (the decompals/old-gcc `gcc-2.7.2-cdk` release, added in #6641 and scoped in
  `notes/overlays/model-variants.md`), plus the North American SU.MRG
  Exodia images `model_image_su_1142_8013b000` and
  `model_image_su_1152_8017b000`, whose helpers end in the CDK
  `addiu $sp; jr $ra; nop` epilogue (see `notes/overlays/exodia-helpers.md`).
  It is not every MODEL variant: the
  Spanish variant images and the USA header-71/201 images keep their
  GCC 2.8.1 profiles, as do the resident and regional executables and all
  other ordinary overlays. The USA MODEL60348/60358/168560/168570 entry, an
  older debug build that prints its work size, selects `gcc_2_8_0_g0_split`
  (the decompals/old-gcc `gcc-2.8.0-psx` release); its only difference from
  2.8.1 there is register choice in a struct-argument packing (see
  `notes/overlays/french-model-image60348-entry.md`). A profile is still chosen by name in the
  `*_matching_c.json` manifests, never by ad hoc flags, and exact full-image
  matching stays mandatory.
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
- Stored pointers (struct/union members, globals without an initializer) take
  `G32` after the `*` (`T *G32 name`, `Typedef G32 name` for pointer typedefs),
  locals walking them are `T *G32 *p`, and calls through them are
  `CALL32(type, f)(args)`; both are no-ops here (see `src/port_ptr.h`). CI
  runs `make check-g32`; `tools/project/check_g32.py --fix` adds missing `G32`.
- Write the Psy-Q 32-bit `long` as `PSXLONG` in code (`PSXLONG x`,
  `unsigned PSXLONG`, `(PSXLONG)v`), never in comments or strings; `long long`
  stays as is. It is `long` here and `int` in a native LP64 build (see
  `src/port_ptr.h`); `make check-g32` rejects a plain `long` and `--fix`
  respells it.
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
