# Script command runtime contracts

`src/game/script_state.h` owns the script engine's command state and its
text-channel entry views. `src/game/menu_record.h` owns the selected menu
record pointer because its complete observed target type already lives there.
These declarations describe accesses, not C storage allocation.

## Evidence and boundaries

`Script_RunTick` fetches the command through `gScript_StreamCursor` and dispatches the
low five bits of `gScript_wCommandWord`. `Text_StageScriptImage` and
`Text_StartViewportTween` in
`duel_effect_state_callbacks.c` install commands 5 and 7 through the same
globals, and `TextBox_BuildStep` dispatches them until the command clears.
Thus the text-channel views belong to the same contract as the script
handlers, not independent copies of the state.

The following access forms remain intentional:

| Consumer | Shared-header selection | Preserved form |
| --- | --- | --- |
| Existing script handlers | Default | Unsigned scalar command, image/shared operands and viewport targets |
| `duel_effect_state_callbacks.c` | `SCRIPT_STATE_TEXT_CALLBACK_VIEWS` | Unsized unsigned arrays; signed `.data` shared operand and scalar aliases |
| `text_box_build_step.c` | `SCRIPT_STATE_COMMAND_IN_DATA` | Unsigned `.data` command scalar |

`Base2_8009B2A8` and `Base2_8009B2AA` remain separate linker identifiers at
the target halfwords' addresses. Their stores occur after calls that the
first target-X/Y stores precede. Do not replace the Base2 identifiers
with C macros or remove the existing scalar `asm` aliases: address retention
and absolute versus small-data addressing are compiler inputs here.

`Script_UpdateViewportTween` initializes `gScript_ViewportAccumulatorX/Y` from the
current viewport in 16.16 form, computes `gScript_ViewportDeltaX/Y` by dividing
the target distance by the signed shared operand, and adds those deltas on
each invocation. The signed
reads used for interpolation and unsigned final halfword copies remain
explicit. These four words do not become one struct: the separately used
command latch and script cursor lie between the two pairs.

`Script_OpWait` seeds signed countdown `gScript_WaitCounter` from two script bytes and
clears the command on the signed nonpositive result. `Script_OpShowImage` and
`Script_OpDuelResult` store a freshly allocated `DisplayObject *` in `gScript_TransitionObject`
and later release it through `DisplayObject_ReleaseIfPresent`. The forward declaration in
`script_state.h` uses the existing `struct DisplayObject` tag; it does not
create a second object layout or import unrelated guarded graphics globals.

## Typed menu command

`func_8002EB78` selects element zero or one of `D_800EB010` using bit 7 of
its second operand. This is the same `0x4C`-byte `MenuRecord` array walked
by `DisplayEffect_ResetMenuRecords` and `DuelEffect_MarkObjectIfActive`, not an inferred new
allocation. Its retained pointer `D_8009B274` now has type `MenuRecord *`.

Seven accesses use the existing members rather than raw byte offsets:
the signed marker at `+0x30`, writes at `+0x31`, `+0x30`, `+0x3C`, `+0x33`
and `+0x34`, and the later `+0x33` completion poll. Compile-time assertions
cover each accessed offset and the unchanged record stride. The signed
marker load, byte stores and halfword store are preserved. Calls into the
older `u8 *` and `DisplayEffectState *` lifecycle interfaces retain explicit
boundary casts; changing those APIs is not required to type the record.

## Migration and exactness

### Execution-state naming checkpoint

`gScript_StreamCursor`, `gScript_EventState` and
`gScript_DialogCommandLatch` name the existing pointer/halfword state
connecting package initialization, command fetching, operands and dialog
replay. The larger runtime bundle also covers retained image/wait and
viewport views; this checkpoint does not publish a separate symbol PR.

The cursor remains `u8 *G32`, not an instruction-only pointer, const
package address or new stream object. Readers keep their separate local
base, cursor advance before byte loads and LE16 expression. Every handler
keeps its original two/four/six-byte advances and branch assignments.
The loaded `D_801A8000` buffer and its unrelated typed users remain
address-qualified rather than gaining a script-only storage interpretation.

The event word remains raw `u16`. `Campaign_LoadScenePackage` copies its
signed argument with the existing halfword store; it does not select a
different package sector. `Script_RunTick` still consumes one RNG value
before all event gates, including the inactive path. Its dialog-active
path pumps channels, clears only the existing bit on the regional
completion equality and returns even when that clear just happened.
Bootstrap uses the original whole pre-start word as the offset-table
index. No script-ID mask, range check or flags-only enum is introduced.

Bootstrap still initializes the cursor, fetches one command byte, clears
the dialog latch and marks event started in the existing order. A later
nonzero latch still replaces the command before dispatch. The interpreter
keeps its unbounded same-call loop: only a nonzero command yields; a
cleared command fetches the next byte immediately. This is not a new
per-frame command budget or queue.

`Script_OpShowDialog` retains the actual uninitialized
`u16 gScript_DialogCommandLatch;` common definition in its original C unit
and the existing owner extern. The named fixed native/linker binding
does not create separate storage or justify converting that definition to
an extern. The captured value remains the complete busy word after
allocation and optional box-retention flag handling, not a Boolean.

Once the separate event dialog bit clears, the replayed handler keeps the
original optional box destruction, latch zero and command zero ordering.
The retain-box variant still skips destruction. Its volatile box-flag
view, regional heights/flags/preparatory call differences and all repeated
state loads remain. No callback, latch reset point or lifetime changes.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Stream cursor | `0x8009B290` | `0x8009B180` | `0x8009C204` | `0x8009C624` |
| Event selector/state | `0x8009B2A4` | `0x8009B194` | `0x8009C208` | `0x8009C628` |
| Dialog command latch | `0x8009B28C` | `0x8009B17C` | `0x8009C202` | `0x8009C622` |

Only seventeen obsolete Japanese cursor/event macros retire. Other
native aliases, every function inventory, profile, manifest, source/header
path, grouped object, initialized table and unrelated package view remain.
All 940 complete resident compiler/MASPSX outputs match after reversing
only these three identities, without filtering instructions, directives,
relocations or filenames. Complete changed C files preserve every other
byte except the exact macro removals; header noncomment tokens preserve
all declaration forms. The accepted ledger gains three evidence/caveat
rows. No tests, fixtures, controls, traces or captures change, and no new
test/harness is added. Regional/cloud validation stays PR-side.

### Retained image, transition-object and wait checkpoint

`gScript_ImageOperand` keeps the full flag-bearing `u16`, not only an
image ID. The ordinary script staging command clears target Y then X,
reads the image word, and optionally consumes X/Y halfwords before
publishing state five. The text staging callback keeps its different
encoding and order: image word, optional Y byte, then X halfword through
the distinct Base2 names. Those encodings are not merged into one decoder.

`Script_OpShowImage` still requests `operand & 0xFFF` and independently
tests `0x4000` for its fade path. `Script_OpLoadImageScene` retains its
conditional behavior: only its `0x8000` optional-coordinate branch
replaces the stored word with `value & 0xFFF`; the other branch passes the
existing complete word. No high-bit clearing, bounds check, value enum or
new transfer interpretation is added. Default scalar and unsized-array
text views keep their exact unsigned halfword declarations/addressing.

`gScript_TransitionObject` remains `struct DisplayObject *G32`.
ShowImage's first invocation still copies the selected frame-buffer
rectangle with `MoveImage` and returns before acquiring the later temporary sprite.
Its subsequent transfer/fade gates, phase stores, image-slot release,
viewport publication, transfer, temporary release and rebuild retain
their order and separate invocations.

DuelResult still retains its regional fill object in that same slot,
with all original allocation, geometry, attribute, color, fade and sound
wait behavior. The two commands release the stored pointer without
nulling it. Naming does not make the pointer live after release, allocate
a new owner, add a cleanup guard or turn it into a general object list.
All variant object configurations and stored integer/pointer views remain.

`gScript_WaitCounter` remains a plain `s16`. The wait command publishes
its two-byte cursor advance, stores the LE16 value through the original
signed halfword, then decrements in that same invocation. Completion
retains the signed nonpositive test on the stored result. Zero and one
can finish immediately; the sign-boundary value keeps the existing
halfword wrap rather than gaining a clamp or positive-duration rule.
There is no viewport-duration alias, frame-step multiplier, widened
counter or newly introduced wait object.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Image operand | `0x8009B270` | `0x8009B160` | `0x8009C210` | `0x8009C630` |
| Transition object | `0x8009B280` | `0x8009B170` | `0x8009C21C` | `0x8009C63C` |
| Signed wait counter | `0x8009B278` | `0x8009B168` | `0x8009C220` | `0x8009C640` |

Only three obsolete Japanese object/wait macros retire. All 940 complete
resident outputs match after reversing only these three names, without
instruction/directive/relocation/filename filtering. Whole C files
otherwise change only by those identities and macros; all noncomment
header tokens retain their declaration forms. Every function inventory,
profile, manifest, source/header path, grouped object, layout, initialized
table and unrelated native/import binding remains. The complete checkpoint
ledger gains three evidence/caveat rows. No tests, fixtures, controls,
traces or captures change, and no test/harness is added. The complete
viewport unit and cumulative clean acceptance still precede one bundled PR.

### Complete viewport and shared-operand checkpoint

`gScript_ViewportTargetX/Y`, `gScript_ViewportAccumulatorX/Y` and
`gScript_ViewportDeltaX/Y` name the existing independent halfwords and
16.16 words, not a newly allocated tween object. StageImage and ShowImage
share the targets with the viewport command and text callbacks. Their
signed interpolation reads, unsigned final copies and retained Base2
address identities remain distinct.

`gScript_SharedOperand` deliberately does not claim universal duration
semantics. The viewport command and text callback stage its divisor/countdown;
`Script_OpDuelResult` also stages its two-byte operand in the same halfword.
The result transfer callback `func_8002F4C0` does not read this operand.
Default consumers retain `u16`; the text callback retains signed `.data`
storage access. There is no new result enum, guard, clearing or lifetime.

The tween's initialization still reads signed current/target coordinates
and a signed divisor, computes `((target - current) << 16) / n` without a
new zero guard, and seeds each accumulator as `(start << 16) | 0x8000`.
The initialization invocation also adds the deltas and publishes X before
Y. It then decrements the shared halfword and tests the signed narrowed
result. Completion clears the command before copying unsigned target X
then Y. No step multiplier, overflow rewrite or minimum duration is added.

`Text_StartViewportTween` first clears the channel state, reads three
halfwords through the original views, then retains separate signed and raw
Y locals. Its test remains signed `Y >= 0x1000`, not a bit test: a value
with bit 15 set does not satisfy that comparison. The subtraction and
state-ten path remain conditional, followed by gate-seven and command-seven
publication in the original order.

The text callback arm declares the targets and `gScript_wCommandWord` as
absolute (`.data`) `u16` scalars; `Text_StartViewportTween` reaches them
through `*(s16 *)&` casts, so no second C name views the same object.
`Base2_8009B2A8/AA` remain separate linker identifiers,
including the UK's symbolic RHS bindings. Nine obsolete Japanese primary
macros retire; no other wrapper aliases change.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Target X | `0x8009B2A8` | `0x8009B198` | `0x8009C20A` | `0x8009C62A` |
| Target Y | `0x8009B2AA` | `0x8009B19A` | `0x8009C20C` | `0x8009C62C` |
| Accumulator X | `0x8009B284` | `0x8009B174` | `0x8009C224` | `0x8009C644` |
| Accumulator Y | `0x8009B288` | `0x8009B178` | `0x8009C228` | `0x8009C648` |
| Delta X | `0x8009B294` | `0x8009B184` | `0x8009C1F8` | `0x8009C618` |
| Delta Y | `0x8009B298` | `0x8009B188` | `0x8009C1FC` | `0x8009C61C` |
| Shared operand | `0x8009B29C` | `0x8009B18C` | `0x8009C1F2` | `0x8009C612` |

All 940 complete resident outputs match after only these seven identities.
Full changed C bodies otherwise differ only by the nine exact wrapper
macro removals. Header noncomment token sequences preserve the scalar,
array, signedness, section, alias and selector forms after the declared
label substitutions. All function inventories, compiler profiles,
manifests, source paths, object groups and unrelated bindings remain.
The complete prior ledger is retained with seven new evidence/caveat rows.
Tests, fixtures, controls, traces and captures are unchanged; no new
test/harness is added and regional/cloud validation remains PR-side.

Tick392 removes 19 private declarations across seven resident translation
units:

| Source | Declarations moved |
| --- | ---: |
| `duel_effect_state_callbacks.c` | 10 |
| `text_box_build_step.c` | 1 |
| `script_update_viewport_tween.c` (now the `Script_UpdateViewportTween` candidate) | 4 |
| `script_control_commands.c` | 1 |
| `script_op_show_image.c` (now `script_image_commands.c`) | 1 |
| `func_8002F630.c` | 1 |
| `func_8002EB78.c` | 1 |

All current matching and candidate sources were searched for these fourteen
linker identifiers; none of the nineteen integrated candidate sources needs
a declaration migration. The narrower resident linker-symbol diagnostic
drops from 99 headerless names / 159 sites to 95 / 155; it does not count all
the moved declarations, including names which already had a shared default
view.

The complete resident image matches after the guarded declarations and typed
menu accesses, with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
No compiler profile, tentative definition, grouped translation unit, assembly
body, register pin, relocation identity or data mapping changes. Candidate
source and fingerprint metadata remain unchanged.

Source-reclassification work must carry the selected header arms if these
functions move to candidate sources. Frontend mode bytes and live save
workspace contracts are separate campaigns, even where their declarations
share a consumer.
