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

The complete first pass now covers all 1,195 resident game functions. Every
remaining compiler-generated game function has a canonical attempt history,
so the default `make candidates` output has no candidate rows.
`--include-partial` intentionally reopens those measured histories and is not
expected to be empty. Future matching work should start from those mismatches
and preserve each new candidate under `tmp/`. PsyQ CRT/SDK functions are not
decompilation candidates and must never be added to `attempts.csv`.

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
`MainMenu_RefreshTradeInventory` (`0x8018338C`) recovered most apparent address
and scheduling tricks by fixing
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

If a function already has a successful external record but was later
reclassified to unmatched assembly, preserve that historical record. Record the
new exact source with `--mode reclassification_match --new-discriminator "..."`
and promote it with `integrate_verified_match.py --evidence-source reclassification`.
This mode requires prior successful external evidence and an unmatched function
at recording time. It adds one new success rather than rewriting the old source
hash or reopening its terminal history. The audit selects the reclassification
record independently of ledger ordering; a subsequent successful
`inline_refinement` of the promoted function can supersede it in turn.

Use `--allow-register-pins` for measured hard-register declarations.
`--allow-symbol-aliases` permits a second C declaration only when its assembler
name exactly matches a symbol in the tracked linker tables; arbitrary
expressions such as `Symbol+0` and unknown names remain rejected. The
allowances are independent and neither permits statement-level inline assembly.

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

Both ledgers are append-only records of what was measured, so a row keeps the
compiler and profile names that were live when it was written. When a profile
is later removed from `compiler_profiles.json`, its name is added to
`RETIRED_PROFILES` in `record_external_attempt.py` instead of being edited out
of the recorded rows: validation continues to accept the history unchanged,
while a new record still requires a profile the manifest currently defines.
Rewriting a recorded compiler name would falsify the measurement, and deleting
the row would erase a terminal result.

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

### Raw byte indexing can be load-bearing

Replacing `*(s32 *)(p + 0x1C)` style access with a member of a struct the tree
already defines is the tidiest-looking change available, and it is not always
free. It changes what the compiler is allowed to assume about aliasing.

`Campaign_LoadScenePackageStage(FileTransferDescriptor *p, s32 stage)` is the
worked example. Every offset it touches
maps onto an existing `FileTransferDescriptor` field, and it calls
`LoadImage2((RECT *)p, ...)`, which confirms the record opens with the `x/y/w/h`
pair. Retyping the parameter and converting all ten accesses does not merely
fail the match, it produces a *better* function: the executable comes out eight
bytes short and the object drops from 1948 to 1940 bytes.

The disassembly locates it. Through `u8 *`, the load of `D_8009B0F4` is pinned
after the halfword stores, because a byte pointer may alias that global:

    sh   v0,4(s0)
    lui  v0,0x0
    lw   v0,0(v0)

Through `FileTransferDescriptor *` it hoists above them, and the mask constants
are then shared across the `switch` arms, which is where the two instructions
go:

    lui  v1,0x0
    lw   v1,0(v1)
    li   v0,832
    sh   v0,48(s0)

So the cast is not always untidy spelling. Where a function stores through a
byte pointer and reads globals in the same basic block, the byte pointer is
what stops GCC from optimising past retail, and the raw access has to stay.
Attempt the conversion per function and measure it; it cannot be applied as a
blanket cleanup.

The converse is the useful half, because it is what makes candidates cheap to
pick. Four conversions since have been byte-exact on the first build:

    func_80037C74      DuelEffectChannel, five fields, no globals
    func_8004318C      DisplayObjectPosition, four fields, no globals
    Dialog_OpenChoice  DuelEffectChannel, five fields, no globals
    func_8003A1EC      MenuRecord, three fields, TWO globals

The last one corrects the filter the first three were chosen with. Selecting
on "the function reads no globals" is too strong and skips work that is
safe. The hazard needs a STORE through the byte pointer for a global load to
be hoisted across; func_8003A1EC only reads through its parameter, so its two
globals cannot be reordered against anything and the conversion is free.

Select on stores through the byte pointer, not on the presence of globals. A
function that only reads through its parameter is safe however many globals
it touches, and a function that stores through it needs the measurement even
if it touches one.

#### The alias rule is the last filter, not the first

Scanning the tree for `*(T *)(base + off)` finds about a thousand sites, and
the store filter above cuts far less of that than it looks like it should.
Working through one batch of read-only candidates, every one was rejected
before the alias question came up, each for a different reason. They are worth
knowing because a scan reports all of them as clean:

- **The file already says the conversion was tried.**
  `ai_script_find_killer.c` opens by recording that it is a
  `-fno-strength-reduce` user, that the walk's reads at +0, +2, +6 and +9 make
  gcc build a second induction variable biased at +2, and that "an index form,
  a struct cursor, dropping the named compare value and inlining the base were
  all tried". The bias belongs to the reducer, not the spelling.

- **The base is a second symbol for storage another symbol already names.**
  `D_800F3A10` is `D_800F2C40[0].field_DD0` and `D_800F56FC` is
  `&D_800F56F0.vrx`; both headers keep the interior symbol deliberately,
  because the matched sites reach the field through it and spelling it as an
  offset from the enclosing object changes which symbol their relocations
  name.

- **The width or signedness of the read does not match the named field.**
  `sound_runtime.c` reads `*(u16 *)(e + 8)` where `SDCommand.field_0008` is
  `s32`, and `e[2]` where `field_0002` is `s16`. Each such site needs a
  `*(u16 *)&...` device to keep its `lhu`, so the conversion buys spelling and
  pays noise.

- **The arithmetic is the function's logic.** In `func_80058434` the base is
  either `&D_800F56F0` or its interior `vrx` symbol depending on a sign, and
  the destination is `base ± 0xC`; the pointer arithmetic is how the function
  swaps which triple is source and which is destination.

- **The stores are in a form the scan did not match.**
  `display_effect_update_callbacks.c` stores with `*(DisplayObject **)p = o`
  and `p[0x33] = 0`, and `library_runtime.c` uses `*(u16 *)(p + 2) += 0xC`.
  A store detector has to cover `*(T **)base =`, `base[i] =` and `+=`, or it
  will hand back store-through-pointer functions as read-only ones.

What survives is narrow and worth stating positively: the conversion is a good
bet when the base is a byte pointer taken to a global that already has a named
type, and every offset read matches a field of that type in both width and
signedness. `func_80058624` is the worked example. It read
`D_800F56F0.vpx` by name and then took `p = (u8 *)&D_800F56F0` to read +8,
+0xC and +0x14 of the same object; `D_800F56F0` is a `GsRVIEW2`, so those are
`vpz`, `vrx` and `vrz`, and naming them was byte-exact on the first build.
A file that reaches a named global through an anonymous pointer is the shape
to look for, and `camera_view.h` records that ten files once did exactly that
to this one object.

### Name the record in one change, reach it in another

Two of these conversions failed in the same shape, and both split cleanly
into a half that lands and a half that does not.

    Password_SetDigitCursorTarget   naming target_x/target_y in
                                    PasswordCursorView and using them: free
    Password_UpdateDigitCursor      the same two fields, plus x, y, timer and
                                    updateFlags, through the same view:
                                    password overlay stops matching

    name_entry_runtime.c            taking display_object_config.h's
                                    DisplayObjectConfigView * prototype, with
                                    casts at six call sites: free
    NameEntry_UpdateGlyphShatter    naming the seven fields it reads through
                                    that view: password overlay stops matching

So a struct member, a prototype, and a cast at a call site are all cheap:
they change what the source says, not what the function does. The expression
a matched function uses to reach memory is not cheap, and both failures were
functions that read one field several times or mix access widths across it --
which is exactly where GCC's choice of base register and reload points is
pinned.

Split the work along that line rather than by file. Land the name and the
declaration, then measure each function that reaches through them
separately; a failure in one function does not cost the naming, and the
record keeps its documentation either way. Both entries above are that split
already: the header now says what the offsets mean while the function bodies
still spell them as retail needs.

## Declaration audits

Collecting duplicated `extern` declarations into headers is driven by scanning
the tree, and a name-based scan of C text mis-reads several real constructs.
Each of these produced a wrong answer during the header-collection campaign
before the source was read:

- **A trailing `__attribute__` sits after the declarator.** `extern u8 X;` and
  `extern u8 X __attribute__((section(".data")));` differ only in the tail, so a
  scan that stops at the name reports the two spellings as unanimous. The
  `.data` spelling is an addressing lever, so that error proposes a change that
  looks safe while hiding the fact that would have stopped it. Capture the text
  between the declarator and the semicolon.

- **`asm()` renames make one object look like two.** Files reach the viewport
  halfwords as `extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")`
  plus a `#define`. The alias identifier is not declared in any header, so it
  scans as an unhoused symbol needing a home, while the real name is already
  declared in `graphics_frame.h` and the divergent spelling is deliberate.

- **A file can opt out of a guarded header declaration.** `save_data_payload.c`
  defines `SAVE_DATA_DECLARE_MASK_STATE_LOCALLY` before including
  `save_data.h`, so the header's copies are suppressed and its local ones are
  the only declarations in scope. A scan that does not evaluate the
  preprocessor sees a file redundantly repeating its own header. That file's
  comment records that the local position preserves the register allocation, so
  deleting the "duplicate" would have been a regression.

- **One declaration can declare several symbols.**
  `extern u8 A[9], B[9], C;` names three, and a regex anchored on the first
  declarator attributes the array bounds to the wrong ones, fabricating
  conflicts that do not exist.

- **Order matters when normalising text.** Blanking string literals before
  extracting `#include "..."` lines empties the include set for every file, and
  a `return D_8009B3EF;` matches a declaration pattern whose type position
  accepts `return`.

A scan for duplicated *type definitions* rather than duplicated declarations
adds two more of its own:

- **The SDK headers repeat layouts on purpose.** `src/psyq` defines many
  structures that are byte-identical to a sibling under another name --
  `CdlLOC` and `DslLOC`, `SndVolume2` and `SpuVolume`, and the whole `SPRT_*`,
  `TILE_*`, `DR_*` and `GsADIV_*` families. That repetition is the published
  interface, so a layout scan has to exclude `src/psyq` before its output means
  anything.

- **An identical layout is not an identical record.** `DuelFieldPosition` in
  `duel_grid.h` and `ScreenPair` in `screen_projection.h` are both
  `{ s16 x; s16 y; }` and describe unrelated memory: the duel cursor, and one
  entry of the projected slot table `D_800EA070`. Merging them would assert a
  relationship that does not exist. The reverse error is available too --
  `ProjectedPair` sits in the same header as `ScreenPair` and differs only in
  that its `x` is `u16` where `ScreenPair`'s is `s16`, so a scan that
  normalises widths to compare shapes reports them as one record and hides the
  single distinction the header exists to record. Duplication worth collecting
  looks like what `screen_projection.h` actually collected: three textually
  identical spellings of one GTE result, in three files, for one address.

The rule the campaign settled on: the scan produces candidates, and reading the
source decides them. Every one of these was caught by reading, and none by the
tool contradicting itself.

### An arity mismatch is measured, not assumed, in either direction

When a declaration and its definition disagree about how many arguments there
are, the two directions are not symmetric and neither is decided by looking.

A caller that sets FEWER argument registers than the callee reads is
repairable exactly when the values the callee reads can be named at the call
site. `src/unmatched.h` records both outcomes for this one direction.

`func_8004CB0C` is the case that cannot be repaired. `model_slot_setup.c`
calls it with no arguments while the callee reads `$a0` through `$a3`, and
only `$a0` is set, so the rest are whatever the register file happened to
hold. There is no expression to write for them, and its `void (void)`
declaration stays.

`func_800540B4` is the same direction and the opposite outcome. A site that
declared no parameters took the definition's true one-parameter signature,
because `$a0` already held the value the caller would have written, so naming
it cost nothing. The missing argument was recoverable, and once it is named
the mismatch is gone.

So the direction does not decide this one either. What decides it is whether
the incoming values can be expressed at the call site: `func_8004CB0C`'s three
extra registers cannot be, and `func_800540B4`'s single one already was.

A caller that passes MORE than the callee reads is the case that looks equally
unfixable and is not. duel_card_effects.c declared `s32 func_8001F364(s32)`
and called it with a flag at both sites; the definition takes void and never
looks at the register. Dropping the argument and the parameter is
byte-identical, so the declaration follows the definition. The instinct that
retail sets $a0 because the declaration says to was wrong here.

One more in the same family, also recorded in unmatched.h: func_80013C28
keeps two incompatible spellings on purpose.

So: an arity mismatch is a measurement, not a reading. Try the definition's
signature at the call sites and build. It costs one build and settles which
of the two directions this instance is.

### A volatile that merely differs can still be the whole match

`notes/build.md` sorts a declaration that disagrees with its definition into
three cases: one that agrees is inert and belongs in the owning header, one
that merely differs may still be inert and is worth a build to find out, and
one that encodes a different view of the address cannot be centralized at all.
The middle case is an invitation to measure, not a presumption that the
spelling is decoration, and it lands on both sides.

`main_services.c` is the published example of it being decoration: three
`extern volatile` declarations argued the volatile held an init block in
source order, and dropping them built byte-identical.

`file_transfer_runtime.c` is the same shape and the opposite answer. It
declares

    extern volatile u16 D_8009B124;
    extern volatile s32 D_8009B0E8;

where `file_stream.c` declares both without the qualifier, and no header owns
either symbol though `file_transfer.h` already owns the rest of that family.
Dropping the two qualifiers does not merely change the encoding; the
executable comes out four bytes short and fails on size alone. The cause is
scheduling, not elimination. `func_80014A5C` stores one word and then tests
the other:

    D_8009B124 = 1;
    if (D_8009B0E8 != 0) {
        return;
    }

With `volatile` the load of `D_8009B0E8` cannot move above the store to
`D_8009B124`, so the load-delay slot in front of the branch has nothing to
fill it:

    sh    v0,0(gp)        # D_8009B124 = 1
    lw    v0,0(gp)        # D_8009B0E8
    nop
    bnez  v0,...

Without the qualifier the load hoists above the store and fills that slot
itself, the nop goes, and the function ends four bytes earlier. That is the
same pinning the `Campaign_LoadScenePackageStage` case above describes for
`D_8009B0F4`, reached from the other direction: there a byte pointer pinned a
global load after some stores, here a volatile store pins a later load after
itself.

An earlier draft of this section explained the four bytes as a dead store
being dropped, reading the guard as two writes in a row. That was wrong, and
worth recording as a way to get this wrong: the `D_8009B124 = 0` arm of the
guard returns immediately, so there is no path on which a following store
overwrites it. The size difference was real and the mechanism invented; the
disassembly is what settled it.

The obvious conclusion from that is the guarded two-arm form `input.h` and
`sound.h` use, one arm per spelling. It is also wrong, and it took a second
build to find out. `file_stream.c` clears the pair once each inside
`File_InitTransferState` and does nothing else with them, so giving *it* the
volatile view costs nothing: one flat `extern volatile` declaration in
`file_transfer.h` serves both files and builds byte for byte.

So a qualifier that one side needs does not by itself force two arms. Ask
which side the difference is load-bearing on, and then whether the other side
is merely indifferent rather than opposed. Two arms are for two genuine
views; a strong spelling and an indifferent one are a single declaration.

The rule to carry: a qualifier difference is worth one build in either
direction, and the build is the whole of the evidence. Neither "it is only a
qualifier" nor "the qualifier must be there for a reason" survives contact
with the two cases above -- and neither does the assumption that a real
difference has to be centralized as two arms.

## Compiler experiments

- Keep probe sources, generated objects, and diffs under `tmp/`.
- Record durable compiler/version/flag conclusions in `notes/toolchain.md`.
- Reuse curated width, stride, signedness, and code-shape evidence from
  `notes/research/matching-evidence.md`.
- Require multiple independent matching samples before selecting a compiler.
- GCC 2.8.1 paired with MASPSX 2.81 is the only pipeline; vary named profiles
  rather than compilers.
- Explore as many materially distinct variants as a function needs. Depth on a
  hard function is worth more than breadth across easy ones.
- The six canonical rows in `config/slus_01411/attempts.csv` are the closed
  historical record of the first campaign, not a quota. Record compiler, flags,
  outcome, and the exact mismatch reason for every experiment; once that ledger
  is closed, continue in `tmp/` and write findings into the function's
  inventory notes.
- Leave the assembly fallback intact until a candidate matches the whole
  executable, and record what was measured so a later session resumes from the
  evidence instead of repeating it.
- Do not infer the compiler solely from the North American release year.
- Do not download or commit proprietary PsyQ binaries. Any user-supplied
  candidate remains ignored under `tools/toolchains/`.

### Measuring a candidate whose instruction count differs

Positional differing-line counts answer "which offsets disagree", which stops
being the useful question as soon as the two sides have different instruction
counts. One inserted or missing instruction shifts everything after it, so the
count then measures the shift rather than the difference, and a side-by-side
window read across the shift is not comparing the instructions it appears to.

`tools/project/align_functions.py <rom_offset> <size> <candidate.o>` aligns the
two instruction streams instead and reports what is inserted or missing. It
strips immediates so alignment keys on opcode and registers only, since a
differing constant should not stop two instructions from pairing. Use it
whenever the counts differ; return to positional tools once they agree, because
alignment deliberately ignores the immediates that a positional diff exists to
catch.

The two instruments answer different questions and neither substitutes for the
other: alignment finds *structure*, position finds *allocation and constants*.

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
