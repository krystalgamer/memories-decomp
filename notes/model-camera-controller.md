# Camera move and follow controller

`Model_UpdateCamera` (`0x80051A48`, `0xAE0` bytes) runs at the end of
`Model_UpdateScene`'s model-scene frame. Its matching C uses the existing
`ModelCameraMove`, `ModelSlot`, `GsRVIEW2` and `Key` owners without adding
storage, aliases, volatile accesses, register pins or executable assembly.
`camera_view.h` supplies its declaration to both definition and caller.

## Retail contract

- All four signed slot selectors are captured before the transfer-status call.
  Negative selectors disable tracking. Nonnegative selectors retain the retail
  unchecked indexing contract; the canonical slot array contains three records.
  No evidence here licenses selectors outside that array.
- In the active-move path, each paired start tracks X/Y only; each end tracks
  X/Y/Z. Signed deltas are limited to `[-30, 30]` before halfword stores.
  Flags select eye and target interpolation independently, including moves
  to explicit coordinates rather than model slots.
- Elapsed advancement halves the signed frame step near either fifth of the
  duration, with a minimum of two in those edge regions. Interior steps are
  not subject to that minimum. The `0xFFFF` duration sentinel skips advancement,
  not tracking, interpolation or the completion comparison.
- Interpolation uses signed full-width differences, products and division,
  then adds the start coordinate. Camera outputs are 32-bit SDK `long` fields.
  Both interpolated and followed Y coordinates have a ceiling of -100.
- An inactive transfer gate or zero flags selects the follow path. Mode zero
  returns without a key lookup. Otherwise end slots follow directly, and orbit
  is suppressed if either requested key endpoint has signed kind 4.
  `Model_GetCurrentEffectKeyAddress` retains its existing integer-address return ABI: the real
  implementation returns zero or the current `Key *` converted to `s32`.
- Orbit retains the real signed direction byte and the existing caller view
  that places an unused fifth argument on the stack. This is not a change to
  the four-argument callee definition.
- Active moves recompute metrics, perform view adjustment and only then check
  completion. Follow-mode countdown may call the frame-step getter twice;
  both field reads are explicitly sequenced after their calls. The second
  subtraction is not independently clamped.

The normal setup in `Model_StartCameraMove` handles a zero-duration request immediately
and clears its flags. This does not establish that every other writer prevents
zero duration: `Model_SetCameraMoveDurationFromMagnitude` writes a duration directly. The controller's
retail division behavior is preserved, without inventing a zero-divisor
fallback or general signed-overflow guarantee.

## Follow countdown and adaptive orbit step

The existing ModelCameraMove halfwords now have evidence-backed names:

| Original | Canonical | Unchanged representation |
| --- | --- | --- |
| field_02 | follow_countdown | u16 at +2 in both layouts |
| field_04 | orbit_step | u16 at +4 in both layouts |
| field_06 | base_orbit_step | u16 at +6 in the full layout ONLY |

Model_StartCameraMove keeps its original 300 default through the
projection-named constant, then baseline/current defaults of eight in
their original order. This is not evidence that the countdown is a
projection, physical time unit or the separate elapsed/duration pair.
The compact Japanese record still lacks the baseline word; later
members retain their two-byte layout difference.

Model_SetCameraFollowCountdown still stores twice its signed request
through the independent signed s16 gModel_wCameraFollowCountdown alias.
The record view stays u16. Preserve the alias, guard, multiplication,
narrowing and independent address construction instead of replacing the
store with typed record indexing or a saturating/absolute-value setter.

Model_UpdateCamera decrements the word only after entering its follow
path with nonzero mode. The first frame-step getter and record read
decide whether the remainder is positive; that true branch calls the
getter AGAIN and reads the word AGAIN before subtracting. Only the other
branch explicitly stores zero. Do not cache either getter/read or clamp
the second subtraction before its original u16 store. Active-move
elapsed advancement returns before this work.

Model_IsCameraMoveComplete still first requires flags == 0. It then
accepts nonzero arg, zero mode OR zero follow_countdown. All established
static callers pass one; no new zero-argument runtime observation is
claimed. Mode, flags and duration retain their distinct protocols.

Both orbit paths multiply orbit_step by the existing live frame step
and signed direction byte for Model_StepViewOrbit's yaw-delta argument.
The follow path still suppresses orbit/correction when either requested
key endpoint is kind four. No degree/radian, wall-clock speed, signed
range, Boolean or guaranteed-positive-step interpretation is introduced.

Model_SetCameraOrbitStep narrows its s32 request to both baseline and
current words in full records, but only current in Japan, under the
unchanged two-state short-circuit guard. The setter remains statically
uncalled; the role follows its body and downstream math, not a capture.
Distance correction calls separation first and clears eye flag bit zero
on a hit. NA adds one when the OLD step is below 0x10; PAL adds two when
the OLD step is below 0x200. When separation is absent, NA subtracts one
only above baseline. PAL retains its u16 subtraction temporary and
post-store comparison against the baseline. Do not turn these into
universal hard caps or a generic nonwrapping clamp. Japan performs
NEITHER adaptive step update and accesses no absent baseline.

All target-slot reads, distance/pitch correction ordering, both pitch
calls, endpoint tracking, sentinel-duration behavior, interpolation,
completion requests, frame getters and mutation boundaries remain.
Only member identifiers change in C. The existing controller fixture
receives inverse-byte-preserving name changes, including its mutation
spellings; every numeric-offset oracle, assertion and scenario remains.
Other fixtures, raw providers, source/profile groups and aliases stay
unchanged. No fixture/cloud execution, harness or new runtime result.

## Historical matching and witnesses

The following witness descriptions record the earlier reconstruction.
Retired gameplay fixtures are not restored or executed by the request/setup
local-view component below.

The full reconstruction replaces the historical incomplete controller spines.
Separate two-element selector initializers recover the `0x38` stack frame.
Separate base and indexed-slot locals, nested conditional clamp additions,
quotient-first direct interpolation, and a scoped elapsed temporary recover
the retail allocation and scheduling. An append-only refinement makes the
countdown getter/load ordering explicit without changing any instruction byte.
The original terminal source snapshot remains separate from that refinement.

`test_model_camera_controller.py` compiles the real C in a separate ILP32
translation unit at `-O0` and `-O2`. Its numeric-offset oracle checks 220
scenarios and 1,040 callback observations, full move/view images, untouched
slot storage and surrounding guards. Cases cover gates, flag combinations,
negative and valid selectors, clamp and easing boundaries, signed direction,
sentinel duration, endpoint-kind exclusions, variable countdown returns and
helper-boundary mutations. Six deliberate source mutations must be rejected.
These scripted mutations stress observable load ordering; they do not claim
that the current leaf frame getter actually mutates the move record.

`test_model_camera_ownership.py` uses GCC 2.8.1 for positive and negative
declaration controls, measured field offsets and widths, real definition/caller
compilation, and the explicit five-argument orbit caller view. Neither suite
replaces clean resident-executable or overlay matching.

## Japanese correction and hold-state providers

The Japanese scene-controller and effect-state wrappers already use the
same three canonical C functions below. Their former private function
macros hid the names from the Japanese function inventory and duplicated
the hold-state binding between its real definition and callers.

| Canonical function | Japanese address (former native label) | Bytes |
| --- | --- | --- |
| `Model_CorrectViewPitch` | `0x80055D7C` (`func_80055D7C`) | `0x16C` |
| `Model_CorrectViewDistance` | `0x80055EE8` (`func_80055EE8`) | `0x1DC` |
| `Model_SetCameraHoldState` | `0x8004EBD0` (`func_8004EBD0`) | `0xC` |

`model_view_adjustments.h` now owns the two correction declarations and
`model_transfer_flags.h` owns the hold-state setter declaration, the same in
every region and with no assembler label; the Japanese symbols.txt binds the
three canonical names at those addresses. Native providers, profiles, grouped object order and all common
C bodies remain unchanged. The three inventory names cover 852 bytes;
they do not add newly decompiled functions.

Distance correction keeps its signed target-slot read, slot-adjusted
700/1000 range, separation call and both pitch-correction calls, one before
and one after the distance steps. Japan only clears flag bit zero on
separation: it does not step `orbit_step` in either direction or access the
absent `base_orbit_step`. The Japanese wrapper therefore retains its compact-layout
selector. NA's one-unit and PAL's two-unit field updates remain independent.
The existing signed distance reads, 150/300 thresholds, step caps, nonzero
`s32` argument test and repeated helper/field accesses are not rewritten.

Pitch correction retains the signed `0x801` test, 3968/3755 interpolation,
target band, `0xEAB` floor and asymmetric orbit steps. The hold-state setter
still assigns its `s32` argument to the existing byte, including truncation;
it is not a Boolean setter. Battle-slot setup can write 2 before animated
battle promotes it to 1. The separate handler-callback byte and the existing
guards requiring both bytes to read exactly 1 are unchanged.

Native conservation covers all six actual source/profile routes for the
complete common, Japanese and PAL scene-controller/effect-state units:
39 functions per image, 24,364 instruction bytes for NA, 24,084 for Japan
and 24,384 for each PAL image. Complete compiler/MASPSX assembly, native
symbols and disassembly/relocations are compared, not isolated replacement
function bodies. Regional executable and witness validation remains on the
PR; this local evidence does not claim regional image execution.


## Guarded request and setup local views

All seven complete model_effect_requests definitions and the complete
Model_StartCameraMove definition now name existing inputs and locals.
These are definition-only argument names: all public declarations, ambient
argument views, integer-address ABI, members and storage remain unchanged.

The two request wrappers retain hold_state and the short-circuited second
getter. The slot wrapper normalizes eye_slot/target_slot independently
BEFORE that first getter: negative becomes zero, otherwise one is added.
mode_request and timing_request are forwarded untouched. The setup routine
STILL captures its view vectors and performs its OWN later guard; no two
guards or getter calls are merged. The zero-endpoint request is not renamed
as an unconditional stop/reset, and no selector clamp or overflow promise
is introduced.

Setup's eye_transport/target_transport remain signed integer scratch.
Positive values are encoded slot selectors, converted to cached-position
addresses; negative nonzero values retain the original explicit-coordinate
transport path without a claim that arbitrary negatives are valid pointers.
Zero falls back to the captured eye_start or view_work address and does not
set that endpoint's move flag. Positive mode with a zero endpoint retains
its old tracking selectors; nonpositive mode still enters selector clearing.
The negative-mode goto still bypasses the mode store at its original site.
selected_slot is the one scratch reused for eye THEN target selection.

view_work deliberately remains mixed: it first holds the live eye vector,
is copied to eye_start, then receives target_copy. Keep both eight-byte
zeroes/copies, component narrowing and all view reads BEFORE the guard.
The six scoped camera_move pointers remain SIX separate bindings; their
common name does not cache or combine accesses.

timing_work retains the original signed input for both pair-slot decisions,
then becomes doubled conditional magnitude. Setup's >FFFE comparison and
FFFF sentinel remain distinct from the direct magnitude setter's unchecked
halfword assignment. The later early return still tests the full-width
timing_work, NOT the stored duration. Negative extreme/overflow behavior,
zero case and narrowing gain no new guarantee or repair.

active_flags captures the original final-block flag read. Only the eye
immediate-write gate uses that capture; target and metric gates STILL read
the later live record flags. Keep the orbit timer reset, elapsed store,
endpoint stores, metric call and final flags clear in their original order.
The projection-named follow default and full/compact orbit-step writes
remain separate from duration, with no Japanese baseline word added.

The two direct selector setters distinguish pair_slot and slot_index but
retain unchecked signed inputs, narrowing and pair-THEN-end store order.
magnitude_request, countdown_request and orbit_step_request retain their
different conversions: conditional magnitude times two with elapsed reset,
signed times two through the independent signed countdown alias, and direct
baseline/current halfword stores (current only in Japan). Guard shapes,
return sites, getter/read order and static reachability claims stay intact.
No wall-clock unit, saturation, Boolean domain or immediate-start promise
is inferred.

Only scoped bare locals/definition arguments and four identifiers in the
existing setup transport comment change. All other C comments, declarations,
types, lifetimes, casts, operators, constants, labels, calls, other grouped
functions, private tables, regional macros, public headers and named profiles
stay exact. The two conversion notes follow their actual local spellings;
the historical witness descriptions do not revive retired gameplay tests.
The entire semantic ledger remains byte-identical: no identity promotion
or NOTES churn. No fixture/control/oracle/mutation edits, local cloud tests
or new behavioral harness accompany these views.
