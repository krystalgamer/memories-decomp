# Matching Decompilation Workflow

## Resource usage

Build concurrency should follow the host's available logical CPUs:

```sh
MAKEFLAGS=-j"$(nproc)" make match
```

Set a smaller `-j` value explicitly on memory-constrained systems. Candidate
selection, candidate compilation pipelines, ledger updates, and integration
decisions remain sequential because their evidence and state updates are
order-dependent.

Any script that adds safe parallel execution should derive its default worker
count from the host and expose an explicit lower worker count.

The Copilot CLI itself has exhausted its JavaScript heap during long,
tool-heavy sessions even when no compiler workers were active. To limit
session-memory growth:

- Do not launch background agents for routine matching or source analysis.
- Use one tool call at a time by default. Do not batch large file reads.
- Search first, then read only narrow ranges. Never load `gms.c`, `dotr.c`, or
  another large reference/export in full.
- Bound command output with focused filters or line limits. Write unavoidable
  verbose logs beneath `tmp/` and inspect only their summary or relevant tail.
- Run candidate verification sequentially and emit one compact result record
  per candidate instead of returning compiler or disassembly dumps.
- Work in small recoverable batches, update durable ledgers immediately, and
  commit each completed batch before loading more reference context. Push
  accumulated commits about every 15 minutes.
- Start a fresh CLI session from the durable notes after a bounded batch if
  memory usage is rising. Do not rely on a single indefinitely resumed
  session as the project state store.
- End the active CLI session after at most one substantial subsystem batch or
  two small atomic commits. If memory forces a handoff before the next normal
  push window, push pending commits immediately, then resume from the tracked
  notes instead of continuing a long conversation.

Every `make` target that enters through `workspace` runs
`tools/project/session_memory_guard.py`. When the parent Copilot CLI reaches
2560 MiB RSS, project commands stop before launching another build. This is a
last-resort guard, not a reason to keep a session alive until the threshold.
CI and ordinary user shells have no Copilot parent and are unaffected.

Do not raise the Node heap to 8 GiB on the current host. It has approximately
8 GiB of physical memory and no swap, so doing so would trade a controlled V8
failure for whole-host memory exhaustion.

## Match invariant

After the assembly baseline, every accepted source change must preserve:

```sh
make match
```

The entire rebuilt `SLUS_014.11` must retain the target SHA-256. A function is
not matching merely because it is functionally equivalent or has a similar
instruction count.

Long integration runs may seed the single-worker incremental object cache after
a clean full match:

```sh
make match
tools/environments/python/bin/python \
  tools/project/build_incremental.py --seed-existing
make match-incremental
```

`make match-incremental` still regenerates the split, relinks the entire
executable, and checks its target SHA-256. It reuses an object only when the
source, local includes, compiler profile, compiler, assembler, MASPSX, assembly
filter, and generated Splat include fingerprints are unchanged. The ordinary
`make match` remains the clean-build acceptance gate for final audits.

## Shared primitive types

Every C source includes `src/types.h`, which is the single definition point
for `s8/u8`, `s16/u16`, `s32/u32`, and `s64/u64`. Do not redeclare those
aliases in a translation unit.

Complex structs, unions, enums, callback types, and uncertain placeholders
remain local until their layouts and ownership are understood well enough for
a separate deliberate header pass.

Validate the convention with:

```sh
make basic-types
```

## Function conversion

For each candidate:

1. Select a small function with a stable boundary and understood callers,
   globals, and data references.
2. Record any naming or type evidence under `notes/`.
3. Move the function into an appropriate C translation unit under `src/`.
4. Keep an exact assembly fallback until the C object matches.
5. Compare instructions, relocations, section placement, and read-only data.
6. Adjust source structure or measured compiler flags without changing
   behavior.
7. Run the full executable match.
8. Commit only that function or an inseparable tightly coupled group.

Progress snapshots are intentionally separate from routine function
conversions. Run `make progress` only when refreshing the project-wide README
metrics.

The tracked inventory is `config/slus_01411/functions.csv`. Reconcile generated
boundaries and ownership before selecting work:

```sh
make classify-functions
make progress
```

The inventory records address, size, current name, status, module ownership,
and durable notes. Its address and size fields must continue to agree with the
generated split.

Select untouched candidates with the guarded project command:

```sh
make candidates
make candidates \
  CANDIDATE_ARGS="--start 0x80028000 --end 0x80038000 --limit 30 --format addresses"
```

The default deliberately excludes every function with any existing attempt
history, not only deferred histories. This keeps automated first-pass batches
from repeating work that already has measurements. Use `--include-partial`
only when intentionally reviewing a historical canonical ledger.

The complete first pass now covers all 1,196 game functions. At this snapshot,
all unmatched functions have deferred canonical histories, so both the default
`make candidates` output and the `--include-partial` result are empty. Future
matching work should start from those measured mismatches and preserve each
new candidate under `tmp/`. PsyQ CRT/SDK functions are not decompilation
candidates and must never be added to `attempts.csv`.

Review deferred histories before continuing deeper investigation:

```sh
make review-deferred
make review-deferred \
  REVIEW_DEFERRED_ARGS="--contains scheduler --limit 20 --format json"
```

The CSV view shows the last recorded result plus measured counts for distinct
compilers, source paths, and tool-error attempts. Those counts expose histories
that spent variants without testing a new source representation. JSON includes
every canonical attempt so interacting changes can be compared together. The
canonical CSV remains a fixed historical snapshot, but it does not limit
source variants, compiler probes, or other investigation preserved under
`tmp/`. Record an exact later result as post-terminal evidence.

### Hypothesis audit before post-terminal work

Treat attempt records as measurements made against one source shape, not as
general compiler laws. Before reconsidering a deferred function:

1. Separate observation from explanation. Record the exact instruction,
   relocation, section, or size difference first. Label its proposed cause as
   a hypothesis unless independent evidence establishes it.
2. Read the complete history together. A change that failed in one surrounding
   source does not establish that it fails after another structural change.
   Test coupled changes when they alter the same dependency, live range, or
   block layout.
3. Reconsider the representation, not only statement order. Recheck
   prototypes, signedness, widths, aggregate dimensions, aliasing, loop form,
   and ownership before tuning a near-match. Preserve the closest reproduction
   under `tmp/`, but do not let it become the only source model considered.
4. Distinguish source, compiler, and layout hypotheses. Compiler-generated
   jump tables or initializers that cannot be placed by the current manifests
   require a build/layout task; more function-local permutations are not a
   meaningful test of that hypothesis.
5. Compare compiler behavior with controlled variables. A profile comparison
   that changes several flags at once does not identify which flag caused the
   result, and a failed set of source permutations does not prove a compiler
   ceiling.
6. Generalize only from repeated evidence. Promote a blocker into shared
   guidance only when the same controlled single-variable change reproduces
   the effect in at least two independent functions, or when one exact match
   is corroborated by direct compiler, linker, or original-source evidence.
   Otherwise preserve the narrow scope in the attempt summary and candidate
   notes.

Two overlay matches demonstrate why this discipline matters:
`func_80184030` required two individually unsuccessful changes together, while
`func_8018338C` recovered most apparent address and scheduling tricks by fixing
its declarations. The resident matching-evidence note likewise records
post-terminal matches unlocked by corrected profiles and source structure.

Record the discriminator explicitly when an audited deferred function reaches
an exact result:

```sh
tools/environments/python/bin/python \
  tools/project/record_external_attempt.py 0x80012345 \
  --mode post_terminal_resolution \
  --profile gcc_2_8_1_g8_split \
  --candidate tmp/candidates/func_80012345.c \
  --result matched \
  --new-discriminator "corrected callback prototype from two matching callers" \
  --summary "complete executable and relocations match"
```

The recorder prefixes the durable summary with the new discriminator. Omitting
it is rejected, so later sessions cannot see a post-terminal success without
the evidence that justified reopening the hypothesis.

For a larger untouched function, find exact-C instruction-shape siblings before
writing a candidate:

```sh
make siblings \
  SIBLING_ARGS="0x80058938 0x80050F24 --top 5"
```

The sibling score compares normalized target instruction n-grams, masks normal
register-allocation differences, and penalizes large size differences. It does
not prove shared semantics, but it identifies matching sources whose branch,
memory, and call shapes are useful starting points. Explicit addresses are
rejected if they already have any attempt history.

## External structural references

External sources under `tmp/references/ygofm-decomp/src/` are source-shape
evidence only. Do not copy or trust their types, declarations, headers,
compiler identity, flags, or build documentation. Recover every declaration,
width, signedness, layout, and profile independently from this project's
binary, callers, relocations, and matching sources.

Record pure-C reference-derived attempts separately:

```sh
tools/environments/python/bin/python \
  tools/project/record_external_attempt.py 0x80012345 \
  --mode reference_match \
  --reference tmp/references/ygofm-decomp/src/func_80012345.c \
  --profile gcc_2_8_1_g8 \
  --candidate tmp/reference-work/func_80012345.c \
  --result nonmatch \
  --summary "Exact structure; local signedness still changes scheduling"
```

`external_attempts.csv` preserves the terminal canonical history in
`attempts.csv`. It is game-only, accepts only pure-C prepared candidates, and
retains the completed campaign rows as historical evidence rather than a
limit on continued work. Use `inline_refinement` for an already matching
function whose GCC asm extensions are being removed. A successful nonmatching
reference candidate is promoted with
`integrate_verified_match.py --evidence-source reference`.

A successful inline refinement atomically replaces its existing source and
profile:

```sh
tools/environments/python/bin/python \
  tools/project/integrate_verified_match.py 0x80012345 \
  --source tmp/reference-work/func_80012345.c \
  --destination src/game/func_80012345.c \
  --profile gcc_2_8_1_g8 \
  --note "Pure-C refinement matched from local declarations" \
  --evidence-source refinement \
  --replace-existing
```

Only the latest successful external candidate for an address is bound to the
current tracked source. Earlier successful reference evidence remains in the
ledger as history if a later pure-C refinement supersedes it.

One-shot imports from the merged Unchiga decomp use a distinct mode so they do
not reset or obscure the historical hypothesis rows:

```sh
tools/environments/python/bin/python \
  tools/project/record_external_attempt.py 0x80012345 \
  --mode collaborator_match \
  --reference tmp/references/ygofm-decomp-unchiga/src/example_unit.c \
  --profile gcc_2_8_1_g8 \
  --candidate tmp/agents/unchiga-integration/candidates/0x80012345.c \
  --result matched \
  --summary "Independently reproduced the collaborator's pure-C match"
```

`collaborator_match` permits exactly one audited row per nonmatching function.
It is not a new search budget: the row records whether the already-matched
collaborator source reproduces under this project's toolchain and declarations.

After a candidate has a terminal `matched` row in
`config/slus_01411/attempts.csv`, promote it with:

```sh
tools/environments/python/bin/python \
  tools/project/integrate_verified_match.py ADDRESS \
  --source tmp/path/to/verified.c \
  --destination src/game/func_ADDRESS.c \
  --profile gcc_2_8_1_g8 \
  --note "Concise matching evidence"
```

The integrator updates the inventory and `matching_c.json`. Generated Splat and
text-object manifests remain under `tmp/generated/` and must not be edited.

## Function status classes

- **Matching C:** compiler-generated game code reproduced from tracked C.
- **Unmatched assembly:** exact fallback that still requires decompilation.
- **Handwritten assembly:** code whose instruction patterns indicate assembly
  was the likely original source model.
- **SDK/library assembly:** identified PsyQ or runtime code retained or replaced
  according to the selected library strategy.
- **Embedded text data:** bytes inside the resident text range that are not part
  of a function.
- **Classified binary data:** mapped non-code regions that do not count toward
  C progress.

Splat's `Handwritten function` classification is an initial heuristic. It must
be corroborated before being treated as final.

## Naming

- Use address-based names such as `func_80012345` and `D_80012345` until there
  is concrete semantic evidence.
- Preserve addresses in notes when renaming symbols.
- Prefer names supported by strings, call relationships, SDK signatures,
  repeated structure accesses, or observed behavior.
- Do not assign speculative subsystem or gameplay names merely to reduce the
  unknown-symbol count.

## Types

- Use fixed-width PSX types and do not rely on the host ABI.
- Recover shared structures from repeated offsets before duplicating local
  placeholder structs.
- Keep pointer/integer conversions explicit and compatible with the 32-bit PSX
  address model.
- Record uncertain fields and competing interpretations in notes rather than
  hiding them with unsafe casts.

## Compiler experiments

- Keep probe sources, generated objects, and diffs under `tmp/`.
- Record durable compiler/version/flag conclusions in `notes/toolchain.md`.
- Reuse curated width, stride, signedness, and code-shape evidence from
  `notes/research/matching-evidence.md`.
- Require multiple independent matching samples before selecting a compiler.
- Try GCC 2.8.1 first. Use GCC 2.7.2 only after the 2.8.1 attempts are recorded
  and exhausted or when code evidence specifically indicates the DOS cohort.
- Pair GCC 2.8.1 with MASPSX 2.81 and GCC 2.7.2 with MASPSX 2.72.
- Attempt no more than six distinct variants for one function.
- Record every attempt in `config/slus_01411/attempts.csv` with compiler, flags,
  outcome, and the exact mismatch reason.
- After the sixth nonmatching attempt, leave the assembly fallback intact and
  defer the function so future sessions do not repeat the same work.
- Do not infer the compiler solely from the North American release year.
- Do not download or commit proprietary PsyQ binaries. Any user-supplied
  candidate remains ignored under `tools/toolchains/`.

## Commit discipline

- Copilot authors and commits its own changes under the `Copilot` name with a
  `+Copilot@users.noreply.github.com` e-mail address. Commits made by other
  contributors keep their own identity.
- Do not add a `Co-authored-by` trailer other than Copilot's own.
- Make one validated logical change per commit.
- Prefer one matched function per commit.
- Keep tool, map, linker, source, and documentation-only changes separate when
  they are independently valid.
- Never amend or rewrite completed commits unless the user explicitly requests
  it.
- Keep commits atomic. Push accumulated commits to `origin/master` about every
  15 minutes while decompilation is active; push sooner only for an explicit
  request or an OOM-risk handoff. Never wait idly for the interval: continue
  the active task and push opportunistically once enough time has elapsed.
