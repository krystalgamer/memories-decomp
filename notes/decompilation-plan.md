# Yu-Gi-Oh! Forbidden Memories Matching Decompilation Plan

The active remaining-function campaign is specified in
`notes/remaining-decompilation-plan.md`. Its phase order is mandatory:
decompilation/refinement, then semantic naming and source grouping, then
cross-function global-use and structure recovery.

Current campaign results and bounded profile-sweep evidence are recorded in
`notes/remaining-decompilation-pass.md`.

The inline-assembly replacement phase is terminal: 21 pure-C replacements were
accepted during the remaining campaign, and all 59 retained inline sources
have complete six-attempt refinement histories.

## Resource usage

- Run Make with `MAKEFLAGS=-j"$(nproc)"` so stronger hosts are not
  artificially capped.
- Set a lower job count explicitly when the host is memory-constrained.
- Candidate decisions, attempt recording, and integration remain sequential
  because their evidence and state updates are order-dependent.

## Collaborator phase snapshot

This section records the completed collaborator-import phase at the point when
its follow-up plan was written. The generated progress section in the root
[`README.md`](../README.md) is the authority for current totals.

- Unchiga pure-C audit: 94 overlapping addresses reviewed, 15 rejected for
  actual inline assembly, 79 independently compiled, 41 exact, and 38
  nonmatching.
- All 41 exact candidates are integrated; 25 replace matching inline assembly
  and 16 add new matching-C functions.
- The semantic registry now contains 175 accepted names after a 19-symbol
  sound/gameplay-state pass.
- Grouped translation units are supported for contiguous, same-profile
  functions. The first two duel groups are documented in
  `notes/grouped-translation-units.md`.
- Subsequent subsystem grouping now covers AI VM/query/control/action/fusion,
  File streaming, Build Deck comparison, Duel rewards, and Main debug code.
- `src/game/ai.h` centralizes the verified active-card, interpreter-state, and
  per-duelist AI layouts; remaining inline-assembly views are deliberately
  deferred.
- `src/game/sound.h` now defines the verified portion of the global
  `g_SDValue` sound-driver work area. Conversion proceeds in exact-matching
  batches, retaining raw local views only where GCC addressing shape requires
  them.
- Snapshot source organization: 773 matching functions in 681 translation
  units; 43 grouped subsystem units contain 135 functions.
- The AI, File, sound-frontend, and sound-driver ranges have no remaining
  contiguous same-profile pure-C runs. Further consolidation requires first
  removing inline assembly, matching intervening functions, or deliberately
  redesigning the object layout.
- Every pure-C user of `g_SDValue` includes `sound.h`. Three raw expressions
  remain solely because typed member syntax changes GCC code generation;
  inline-assembly users remain deferred.
- The remaining-function campaign is terminal: all unmatched game functions
  have six-attempt histories, and all retained matching inline sources have
  six-attempt refinement histories.
- Post-campaign global-use analysis covers 3,356 function/global rows and 784
  globals across 923 game functions.
- Shared post-campaign layouts now cover the secondary sound state,
  DuelEffect channels/entries, model slots/handler registry, duel-card records,
  and fade transitions in addition to the earlier AI and primary sound state.

## Problem and proposed approach

Create a matching decompilation project for the North American PSX executable
`game/SLUS_014.11`. The project will incrementally replace exact assembly
fallbacks with matching C while preserving a byte-identical rebuilt PS-X EXE at
every accepted step.

The work will begin by freezing and verifying the original executable, making
all project-managed tooling local to `tools/`, identifying the compiler/linker
instead of assuming a PsyQ release, mapping the executable, and establishing an
exact assembly/data baseline. Decompilation then proceeds function by function,
with full-file comparison as the merge gate.

## Confirmed scope and constraints

- Target only the current North American `SLUS-01411` build.
- The primary acceptance target is a byte-identical rebuilt PS-X EXE.
- Incremental unmatched assembly is allowed until functions are converted to
  matching C.
- Do not decompile the 598 PsyQ CRT/SDK functions. Keep them as exact assembly
  and exclude their addresses from the attempt ledger.
- Treat every file under `game/` as an immutable, user-supplied input.
- Keep game binaries local and untracked; commit only hashes, metadata, and
  instructions needed to validate user-supplied files.
- Do not rebuild a disc image or reverse the data archives as project goals.
  Inspect a data format only when executable analysis requires it.
- Use the original BIN/CUE as an immutable LBA and streaming-layout reference.
  Verify extracted files against their ISO9660 extents without expanding scope
  to a full-disc rebuild.
- Treat MRG files as script-concatenated data whose generated offsets are
  expected in compiled executable tables, not as self-describing archives.
- Treat the high-memory destinations as shared runtime slots. Resident loader
  traces currently tie WA to `0x80146000`, `0x80168000`, and the data subrange
  at `0x8017A1D8`; MODEL to executable content at `0x8013A000`; and SU to the
  observed modules at `0x80180000`.
- Put all project scripts, downloaded tools, compilers, runtimes, environments,
  and third-party sources under `tools/`.
- Put all detailed documentation, research, naming notes, and curated reports
  under `notes/`. Keep only the generated project overview in the root
  `README.md`.
- Persist the approved implementation plan at
  `notes/decompilation-plan.md` and update that tracked copy during future plan
  iterations.
- Put all generated builds, caches, logs, scratch analysis, downloaded staging
  files, and other temporary work under `tmp/`.
- Commit implementation work continuously in small, independently understandable
  and verifiable changes rather than accumulating a large uncommitted batch.
- Attempt at most six distinct source/compiler variants for one function.
  Record every result in `config/slus_01411/attempts.csv`; defer the function
  after the sixth nonmatching attempt.
- Keep commits atomic and push accumulated commits to `origin/master` about
  every 15 minutes during active decompilation. Continue working between push
  windows rather than waiting for the timer.
- Attribute every implementation commit you make solely to the `Copilot` name
  with a `+Copilot@users.noreply.github.com` e-mail address. Do not attribute
  your commits to the user or add a `Co-authored-by` trailer for anyone else.
- Run every project command from the current repository root. Do not change to,
  read project inputs from, or write project artifacts into parent, sibling, or
  unrelated directories.
- No original symbols, map files, source fragments, debug data, BIN/CUE image,
  or confirmed original compiler are currently available.
- Psy-Q 4.6 is the selected SDK version based on the user's independent
  multi-tool verification. Its Win32 tools use GCC 2.8.1, while the bundled DOS
  tools use GCC 2.7.2. Always try 2.8.1 first and retain 2.7.2 as the fallback.
  Pair them with MASPSX 2.81 and 2.72 respectively.
  The unusual `LIBDS.LIB` was an online patch distributed before Psy-Q 4.7,
  which explains why library-only identification can associate it with 4.7.

## Current state

### Repository

- Git history contains small, atomic commits. Every commit Copilot makes is
  authored and committed as Copilot; commits from other contributors keep
  their own identity.
- User-supplied files under `game/` are immutable, ignored, and hash-validated.
- Pinned Python analysis tools and GNU binutils 2.42 for `mipsel-none-elf` are
  installed locally under `tools/`.
- A pinned open-source `mips-sony-psx` GCC 2.8.1 probe compiler is installed
  locally with explicit PSX flags; it is not treated as Sony CCPSX.
- The executable has a validated region map, Splat configuration, linker
  script, exact assembly/data build, ownership classifications, and a tracked
  resident-function inventory in `config/slus_01411/functions.csv`.
- `make match` reproduces the target SHA-256 exactly, and `make audit` verifies
  repository policy and clean deterministic regeneration.
- `functions.csv` is the authority for current ownership and matching status;
  the root `README.md` generates the current progress totals from that
  inventory. Historical campaign snapshots remain dated in their dedicated
  sections rather than being repeated as live status here.
- The attempt ledgers under `config/slus_01411/` record bounded
  source/compiler experiments for game-owned functions. PsyQ functions remain
  outside that decompilation process.
- Further conversion uses 2.8.1 first and falls back to GCC 2.7.2 only when
  recorded evidence points to the DOS cohort; a function is deferred after six
  unsuccessful variants.

### Target executable

| Property | Value |
|---|---|
| Path | `game/SLUS_014.11` |
| Format | North American Sony PlayStation executable |
| File size | `0x1D0800` / 1,902,592 bytes |
| Header size | `0x800` bytes |
| Loaded payload size | `0x1D0000` / 1,900,544 bytes |
| Load address | `0x80010000` |
| Entry point | `0x800129D8` |
| Initial stack | `0x801FFFF0` |
| SHA-256 | `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88` |

Useful embedded evidence includes:

- Memory-card identifier `BASLUS-01411-YUGIOH`.
- Original source hints `S3000000.C` and `src/hirata/H_mctrl1.c`.
- PsyQ library RCS strings for `intr.c`, `bios.c`, and `sys.c`, dated from
  1997 through 1998.
- Runtime references to all seven files in `game/DATA/`.

These strings can seed library identification and source-module boundaries, but
they do not prove the compiler or SDK release.

### Immutable data inputs

The extracted data set contains:

- `game/DATA/MASTER.XA`
- `game/DATA/MODEL.MRG`
- `game/DATA/MOVIE.STR`
- `game/DATA/SD_BGM.DAT`
- `game/DATA/SD_SE.DAT`
- `game/DATA/SU.MRG`
- `game/DATA/WA_MRG.MRG`

The original disc reference also contains:

- `game/rpg-yfm.bin`
- `game/rpg-yfm.cue`

Their sizes and SHA-256 hashes should be recorded in the target manifest even
though only the executable is rebuilt. The executable's references to these
files make their identity useful for validating the local game version.

The BIN/CUE additionally anchors ISO9660 LBAs and verifies that each extracted
file is byte-identical to its original disc extent.

## Intended repository layout

```text
.
├── .gitignore
├── Makefile
├── asm/
│   ├── nonmatchings/
│   └── data/
├── config/
│   └── slus_01411/
│       ├── target.yaml
│       ├── files.sha256
│       ├── split.yaml
│       ├── symbols.txt
│       └── undefined_syms.txt
├── game/                       # Immutable, ignored user inputs
├── include/                    # Project headers
├── linker/                     # Linker scripts and section-order inputs
├── notes/                      # All human documentation and research
│   └── decompilation-plan.md   # Durable copy of the approved plan
├── README.md                   # Overview with generated current progress
├── src/                        # Matching C sources
├── tmp/                        # Every generated or temporary artifact
└── tools/
    ├── bin/                    # Local entry points/wrappers
    ├── bootstrap/              # Tool bootstrap logic and lock metadata
    ├── project/                # Project-owned analysis/build utilities
    ├── toolchains/             # Candidate and selected local toolchains
    ├── vendor/                 # Pinned third-party source/install trees
    └── environments/           # Local Python/runtime environments
```

No executable project script should be placed at the repository root. The root
contains only the stable `Makefile` interface and `README.md` overview; scripts
must remain beneath `tools/`. Generated assembly listings, split output, object
files, rebuilt executables, compiler experiments, caches, and reports remain
under `tmp/`; only reviewed source assembly and metadata move into tracked
directories.

## Plan persistence and future iterations

- During plan review, the session plan remains the working source of truth.
- Immediately after the plan is approved, copy its approved content to
  `notes/decompilation-plan.md` before making other repository changes.
- Treat `notes/decompilation-plan.md` as the durable repository record used by
  future sessions and implementation iterations.
- When requirements, milestones, risks, or directory policies change, update
  both the active session plan and the tracked notes copy so they do not drift.
- Commit each meaningful plan revision as its own documentation-only atomic
  commit, with Copilot as the sole author and committer.
- Do not place another copy at the repository root; all persistent planning
  documentation belongs under `notes/`.

## Git history policy

Git history is part of the project deliverable and must make progress easy to
inspect and bisect:

- Configure the repository-local Git author and committer identity to the
  `Copilot` name with a `+Copilot@users.noreply.github.com` e-mail address
  before the first implementation commit.
- Do not add `Co-authored-by` or other contributor trailers for anyone other
  than Copilot.
- Make one commit for each complete logical change, normally after its targeted
  validation passes.
- Keep scaffolding, tool bootstrap, binary metadata, split-map changes, linker
  changes, matched function conversions, and documentation corrections in
  separate commits unless they are inseparable for correctness.
- Prefer one matched function per commit. A tightly coupled function group may
  share a commit only when separating it would leave a broken build or obscure
  the actual matching change.
- Include regenerated symbol/progress metadata in the same commit as the code
  change that caused it, while excluding transient reports under `tmp/`.
- Require every commit after the exact baseline milestone to preserve the
  full-file match. Before that milestone, each commit must preserve the stated
  acceptance criteria for its phase.
- Use concise commit subjects that describe the outcome, such as
  `build: verify SLUS-01411 inputs`, `config: map executable entry region`, or
  `decomp: match card initialization helper`.
- Never amend, squash, or rewrite completed commits unless the user explicitly
  requests it. Fix mistakes with a new focused commit.
- Inspect staged changes before every commit so unrelated files never enter the
  same change.

Planned initial commit boundaries are:

1. Add the approved plan as `notes/decompilation-plan.md`.
2. Add ignore rules and the immutable target manifest.
3. Add input verification and workspace path guards.
4. Add the pinned local tool bootstrap and lock metadata.
5. Record compiler-fingerprint probes and conclusions.
6. Add executable split configuration and initial symbols.
7. Add linker configuration and the byte-identical assembly/data baseline.
8. Add matching workflow, progress tooling, and contributor-facing notes.
9. Commit each subsequent matching C conversion independently.

## Implementation plan

### 0. Persist the approved plan under `notes/`

Before other repository implementation:

- Copy the approved session plan to `notes/decompilation-plan.md`.
- Confirm that the durable copy contains the agreed target, directory rules,
  matching definition, toolchain-identification gate, Git policy, milestones,
  risks, and acceptance criteria.
- Commit only that notes file in a documentation-only commit attributed solely
  to Copilot.
- Use this file as the starting context for future planning sessions, updating
  it whenever an approved plan revision changes implementation direction.

Acceptance criteria:

- `notes/decompilation-plan.md` exists and reflects the approved plan.
- Its initial commit contains no scaffolding, tools, binaries, or unrelated
  files.

### 1. Freeze the immutable input baseline

Create the repository safety and version metadata before performing any
decompilation:

- Add `.gitignore` rules for the supplied files under `game/`, all of `tmp/`,
  installed/downloaded proprietary toolchain material, local caches, and other
  generated artifacts. Keep project-owned scripts and lock metadata trackable.
- Record the exact size, SHA-256, PS-X EXE header fields, and expected filenames
  in `config/slus_01411/target.yaml` and `files.sha256`.
- Add `tools/project/verify_inputs` to fail clearly when a file is absent,
  renamed, modified, or from another revision.
- Keep `game/SLUS_014.11` read-only during all extraction and build operations.
- Add a target such as `make verify-inputs` as the prerequisite of every
  extraction, build, diff, and progress command.

Acceptance criteria:

- The current files pass validation.
- A one-byte executable change, wrong size, or missing data file fails before
  any generated output is produced.
- Git does not stage any supplied game binary.

### 2. Enforce the workspace-only filesystem contract

Build path safety into the project instead of relying on convention:

- Make entry points require execution from the repository root and verify that
  the current directory contains `.git`, `game/SLUS_014.11`, and the target
  manifest.
- Resolve every writable path before use and reject any path outside the
  current repository root.
- Set `HOME`, `TMPDIR`, `XDG_CACHE_HOME`, package caches, compiler temporary
  directories, and similar environment variables to locations under `tmp/`.
- Direct downloads and permanent tool installations to `tools/`; use `tmp/`
  only for download staging and extraction scratch space.
- Avoid global package installation, user-level package installation, and
  writes to home-directory or system caches.
- Make cleanup delete only explicitly known subdirectories beneath `tmp/`.

Acceptance criteria:

- Normal commands create or modify files only under this repository root.
- Commands fail before work begins when launched from another directory or
  when configured with an external output/cache path.

### 3. Bootstrap a pinned local analysis and build toolset

Create a repeatable tool bootstrap whose installed state is wholly contained in
`tools/`:

- Add a lock manifest containing tool names, exact versions or commits,
  download/source hashes, build options, and licenses.
- Install a local Python runtime/environment and all Python packages under
  `tools/environments/`.
- Install a pinned MIPS assembler, linker, `objdump`, and `objcopy` under a
  project-local toolchain prefix.
- Install a PSX-aware splitter/disassembler stack, with `splat` or an equivalent
  frontend and `spimdisasm`/Rabbitizer-class decoding support.
- Install an assembly differ and optional decompilation helpers such as `m2c`
  and a permutation tool under `tools/`.
- Add a project-owned PS-X EXE header parser/packer if the selected linker does
  not emit the required executable container directly.
- Keep optional heavyweight static-analysis software and its runtime local
  under `tools/` if it is needed for cross-reference recovery.
- Never download proprietary PsyQ components. Candidate PsyQ files must be
  lawfully supplied by the user, placed beneath an ignored
  `tools/toolchains/` input directory, and verified by hash.

Expose stable root commands such as:

```text
make tools
make verify-inputs
make extract
make build
make match
make diff
make progress
make clean
```

Acceptance criteria:

- A bootstrap starting from the tracked project files installs all
  project-managed dependencies beneath `tools/`.
- Build and analysis commands resolve their project tools from `tools/`, not
  accidental host installations.
- The bootstrap performs no global or user-level installation.

### 4. Identify the compiler, assembler, linker, SDK libraries, and flags

Treat toolchain identification as a measured investigation:

- Select several independent functions with clear boundaries and varied
  code-generation traits: small leaves, stack-heavy functions, switches,
  signed/unsigned arithmetic, structure accesses, and call-heavy routines.
- Document prologue/epilogue forms, delay-slot scheduling, register allocation,
  branch idioms, switch-table layout, small-data use, and emitted helper calls.
- Build a small local compiler probe corpus with the Psy-Q 4.6 Win32 GCC 2.8.1
  toolchain first. Keep the Psy-Q 4.6 DOS GCC 2.7.2 toolchain ready as the
  fallback, together with reproducible open-source equivalents.
- Compare generated instruction sequences and object/link behavior rather than
  relying on game release dates or SDK library RCS dates.
- Identify likely PsyQ library functions separately from game-owned code and
  record library revisions or object signatures where evidence supports them.
- Record the selected compiler binaries, hashes, flags, assembler mode, linker
  behavior, and per-file exceptions in `config/` and `notes/toolchain.md`.

Decision gate:

- Select a C toolchain only after it reproduces multiple independent code
  samples and relevant relocation/link behavior.
- If no candidate matches, continue with the exact assembly baseline while
  documenting the missing evidence. Do not force a functional-but-nonmatching
  compiler into the primary build.

### 5. Map and split the executable

Create a complete address-space model before broad C conversion:

- Parse the `0x800`-byte PS-X EXE header and extract the loaded image to
  `tmp/extract/`.
- Configure virtual addresses from `0x80010000` through the end of the loaded
  payload and preserve the entry point at `0x800129D8`.
- Discover function starts, jump tables, strings, pointer tables, alignment
  padding, initialized data, small data, and probable BSS/runtime-clear ranges.
- Audit for dynamically loaded code or overlays. The archive references appear
  asset-oriented, but this must be proven before treating the executable as
  wholly monolithic.
- Separate likely PsyQ runtime/library code from game-owned modules using RCS
  strings, signatures, call patterns, and known SDK implementations.
- Seed `symbols.txt` with conservative address-based names, then add evidence-
  based names without inventing unsupported semantics.
- Capture source-path hints and proposed module groupings in
  `notes/function-map.md`.
- Ensure every byte in the loaded payload is assigned exactly once to code,
  read-only data, writable data, padding, or another explicitly named region.

Acceptance criteria:

- The split configuration round-trips the entire loaded payload without gaps,
  overlaps, or unclassified bytes.
- Function and data boundaries are sufficient to produce relocatable assembly
  objects and a deterministic linker order.

### 6. Establish a byte-identical assembly/data baseline

Before claiming decompilation progress, make the project rebuild the target
exactly:

- Generate exact MIPS assembly for unmatched code with explicit
  ordering/delay-slot directives and stable labels.
- Represent initialized data, strings, jump tables, alignment, and padding in a
  form that reproduces their original bytes and addresses.
- Reconstruct section order and linker behavior in `linker/`.
- Emit the PS-X EXE header with the original load address, entry point, payload
  length, stack fields, region text, and padding.
- Add `tools/project/match` to compare file size, full-file SHA-256, and the
  first differing offset/range.
- Allow opaque binary inclusion only as a temporary diagnostic bootstrap.
  Granular code/data accounting is required before the assembly baseline is
  considered complete, and opaque original bytes do not count as decompilation
  progress.

Acceptance criteria:

- `make build && make match` produces `tmp/build/SLUS_014.11` with SHA-256
  `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
- A clean rebuild is deterministic.
- Every original payload byte is attributable to a named build input.

### 7. Define the matching-C workflow and project conventions

Make function conversion repeatable and measurable:

- Define C types for PSX fixed-width integers, addresses, hardware structures,
  graphics/audio primitives, and game data without relying on host ABI sizes.
- Establish naming rules for unknown functions/globals, evidence-based renames,
  translation-unit boundaries, and declarations.
- Record global and per-file compiler flags; avoid hidden one-off command-line
  changes.
- Keep unmatched functions in `asm/nonmatchings/` and replace one function or a
  tightly coupled group at a time.
- Use assembly diffs to diagnose register allocation, instruction selection,
  scheduling, rodata placement, and relocation mismatches.
- Require the full executable to remain byte-identical after every accepted
  conversion.
- Track statuses separately for matching C, justified SDK/handwritten assembly,
  unmatched assembly, data, and unidentified regions.

Acceptance criteria:

- A contributor can select an unmatched function, build it as C, inspect a
  focused diff, and either obtain a match or revert to the exact assembly
  fallback without breaking the full build.

### 8. Decompile incrementally by evidence-backed modules

Use an order that improves symbols and types while preserving matching:

- Start with small leaf routines, obvious libc/PsyQ routines, assertion/debug
  helpers, and functions anchored by unique strings.
- Recover common structures and global types from repeated access patterns
  before tackling large control-flow-heavy functions.
- Group functions into translation units using call locality, shared globals,
  string clusters, alignment boundaries, and the surviving source-path hints.
- Prioritize central initialization, memory/card I/O, CD/filesystem access,
  graphics, audio, model/stage loading, and game-state modules once foundational
  types are stable.
- Reclassify SDK/library code when strong signatures become available; preserve
  genuinely handwritten or library assembly when C reproduction is not the
  correct source model.
- Inspect `game/DATA/` structures only when needed to type executable code.
  Do not expand the project into an asset extraction/repacking effort.

Completion target:

- All game-owned compiler-generated functions are matching C.
- Any remaining assembly is explicitly classified and justified as
  handwritten, SDK/library, or otherwise not reasonably represented as
  original game C.
- The full executable remains byte-identical.

### 9. Maintain detailed documentation under `notes/` and progress in README

Create and update:

- `notes/setup.md` for lawful input placement, input validation, local tool
  bootstrap, and build commands.
- `notes/toolchain.md` for compiler/linker evidence and selected versions.
- `notes/memory-map.md` for sections, address ranges, BSS, and hardware-facing
  regions.
- `notes/function-map.md` for module hypotheses, source-path evidence, and
  naming decisions.
- `notes/build.md` for the exact build/link pipeline and troubleshooting.
- `notes/progress.md` for current matching metrics and milestone summaries.
- `notes/research/` for curated subsystem investigations.
- Root `README.md` for the generated high-level project status and contributor
  entry points.

Generate transient HTML, CSV, JSON, diff, and graph reports under `tmp/`; move
only reviewed, durable conclusions into `notes/`. Regenerate the marked README
progress section from authoritative metadata with `make progress`, and reject
stale values in CI and `make audit`.

Acceptance criteria:

- Detailed documentation never appears outside `notes/`; the root `README.md`
  is the sole project-overview exception.
- Reported progress is generated from the build configuration and symbol map,
  not manually estimated.

### 10. Prove clean reproducibility and final completion

- Delete only the known generated directories under `tmp/`.
- Re-bootstrap the pinned local tools under `tools/` from the lock metadata.
- Revalidate the user-supplied files under `game/`.
- Run extraction, build, full-file match, and progress generation from the
  repository root without accessing another project directory or writing
  outside this workspace.
- Verify the final output size and SHA-256 against the frozen target.
- Audit that no game binaries, proprietary toolchain files, caches, or generated
  build products are tracked.
- Record the final compiler/tool hashes, remaining justified assembly, and
  progress totals under `notes/`.

Final acceptance criteria:

- A clean local run rebuilds `SLUS_014.11` byte for byte.
- All project-managed tools are under `tools/`, detailed documentation is under
  `notes/`, the generated overview is in root `README.md`, all
  temporary/generated work is under `tmp/`, and all original game files remain
  immutable and untracked under `game/`.
- `notes/decompilation-plan.md` contains the current approved implementation
  plan for future sessions and iterations.
- The project contains no unexplained unmatched game code or unclassified
  executable bytes.
- Git history consists of small validated commits. Copilot's own commits are
  attributed to Copilot, carry no foreign co-author trailers, and mix in no
  unrelated changes.

## Key risks and handling

| Risk | Handling |
|---|---|
| Psy-Q 4.6 contains Win32 GCC 2.8.1 and DOS GCC 2.7.2 cohorts, while the pre-4.7 `LIBDS.LIB` patch obscures library provenance | Keep both compilers available, try 2.8.1 first, switch to 2.7.2 only after recorded evidence, hash supplied artifacts, and retain exact assembly until each cohort is matched. |
| SDK library dates are mistaken for game compiler dates | Treat RCS strings only as library evidence and corroborate with generated code/object behavior. |
| No map or symbols exist | Use conservative address-based names, cross-references, string anchors, signatures, and documented evidence-based renames. |
| Section/BSS boundaries are not explicit in the PS-X header | Infer them from address references, initialization loops, alignment, linker experiments, and complete byte accounting. |
| Hidden code overlays exist | Audit load destinations and archive/CD reads before finalizing the monolithic split. |
| A trivial whole-binary include gives a false 100% match | Exclude opaque original-byte inclusions from progress and require granular code/data ownership for the baseline. |
| Proprietary/copyrighted inputs enter Git | Ignore game and proprietary toolchain binaries, verify user-supplied copies by hash, and commit only metadata and original project work. |
| Tools write to global caches or outside the workspace | Wrap every entry point with root/path guards and local `HOME`, temp, and cache environment variables. |
| Large or mixed commits make progress difficult to audit | Stage only one logical change, validate it, and commit immediately with Copilot as the sole author and committer. |
| The session plan and durable project plan drift apart | Copy the approved plan to `notes/decompilation-plan.md`, update both on future revisions, and commit plan changes separately. |

## Todo dependency order

1. Configure Copilot-only repository attribution and begin the atomic commit
   discipline.
2. Persist the approved plan at `notes/decompilation-plan.md`.
3. Freeze the immutable input baseline.
4. Enforce workspace-only path and cache rules.
5. Bootstrap the pinned local tools after guardrails exist.
6. Identify the compiler/linker and map the executable in parallel once the
   baseline and tools are available.
7. Establish the byte-identical assembly/data build after both toolchain
   evidence and the executable map are sufficient.
8. Define the conversion workflow on top of the exact baseline.
9. Perform incremental matching decompilation while maintaining documentation
   and generated progress metrics.
10. Run the clean reproducibility, history-audit, and final-classification
    gate.
