# Model camera orbit presets

## Direct and preset application

`Model_SetCameraOrbit` is the former `func_8005F3B8`. The existing live-view
and metric getters still run before the two-byte effect-state guard.
When permitted, a nonnegative slot selects the reference point through
`Model_CopySlotPosition`; a negative slot retains the current point.
An optional `SVECTOR` offset keeps the original side-dependent signs.

The helper then writes distance, biased yaw and pitch into the existing
metric view, forces orbit recomputation with a raw full-turn argument,
captures the keyframe pose and requests camera move `-1`, in that order.
The `int` definition and compatible `s32` public declaration stay as they
were; no shared type, narrowing, remainder or guard policy is changed.

`Model_SetCameraOrbitFromPreset` is the former `func_8005F27C` (Japanese
`func_8004ECCC`). It selects a record before its ordered guard, takes the
base distance, conditionally applies the existing slot adjustment and
passes the record's yaw/pitch fields into the direct setter. The matching
parallel distance expressions are not extracted or normalized.

Both functions remain in the same grouped state source, at their original
native sizes `0x1AC` and `0x13C` and named `gcc_2_8_1_g8_split` profiles.
That earlier component retired only obsolete Japanese provider/caller
preset-wrapper aliases. Its then-existing intro-controller control changed
one identifier, preserving all values, assertions, stubs and paths; this is
historical evidence, not a request to restore retired gameplay fixtures.

## Coefficient, distance and yaw queries

`ModelEffect_GetCoefficient` retains the canonical name already accepted
for the Japanese getter and replaces the NA `func_8005F1A4` spelling. It
still returns the indexed `ModelCameraOrbitPreset *` without a new bound.
Exactly three obsolete Japanese name bridges retire; the Japanese native
function identity itself is unchanged.

`Model_AdjustCameraDistanceForSlot` is the former `func_8005F1B8`.
The first argument indexes a model slot, not an inferred gameplay level.
The original `slot >= 2` passthrough remains. Other inputs retain the
existing maximum adjustment, minimum fifty, halfword subtraction by three
hundred, signed narrowing, delta-dependent signed halving and divisor
seven hundred fifty. Parallel preset expressions are not rewritten to
call this function.

`Model_GetSlotCameraPresetYaw` is the former `func_8005A618`. Its existing
CF8 interior view and byte selector choose a record through the getter;
the yaw halfword receives the same quarter/three-quarter-turn side bias
as the direct setter. Repeated base construction, slot stride, selector
mask `0x1F`, signed remainder and nonpositive/positive branch stay exact.

The model-slot-state Lua trace deliberately keeps its numeric function
labels, breakpoint/return addresses, messages, observations and controls
unchanged. These static names do not claim a new trace run or settle the
selector byte's broader gameplay meaning.

The selector bytes are now named guardian_star_options in the CF8
prefix. They pack low-five-bit preset selection with separate camera
and draw options; they are not a pure preset-index array. The existing
direct-yaw index, animated-battle remapping and all unchecked accesses
remain distinct. The paired battle_camera_duration_overrides and tail
property mirrors are documented in [the metadata protocol](model-structures.md#guardian-star-presentation-metadata).

## Initialized orbit presets

`gModel_aCameraOrbitPresets` is the former
`tent_ModelEffectCoefficients`. The direct setter establishes the roles
of the first three halfwords as distance, yaw and pitch. The recovered
`ModelCameraOrbitPreset` name retains the existing representation;
the fourth halfword still has no established semantic role.

All fifteen initialized eight-byte records remain byte-for-byte and in
their original order, beginning at NA `0x80091570`. The public declaration
stays unsized under the same custom-extern selector. The slot selector's
`0x1F` mask is not an entry count and can reach beyond the recognized
fifteen; no bound, count, clamp, padding or new allocation is introduced.

The obsolete source comment describing a gameplay level/stat or
positional sound is corrected from the actual camera consumers:
`Model_QueueRelativeCameraOrbit` queues relative orbit endpoints through the existing key
queue. Its implementation and the parallel adjustment expressions stay
untouched; only the unrelated sound interpretation is removed.

## Recovered preset record

The old ModelEffectCoefficient spelling is now ModelCameraOrbitPreset.
Its first three members are named from the complete direct/queued orbit
chain, not from a guessed coefficient or gameplay-stat interpretation:

| Original | Canonical | Unchanged representation |
| --- | --- | --- |
| field_00 | distance | s16 at +0 |
| angle | yaw | s16 at +2 |
| field_04 | pitch | s16 at +4 |
| field_06 | field_06 (unknown) | s16 at +6 |

Size remains eight bytes. Existing size/yaw/pitch assertions and added
halfword-width/distance/fourth-word checks preserve the old layout.
No member becomes unsigned, a pointer, a VECTOR or a packed aggregate.
The fourth word stays unknown, not newly declared padding or a kind.
The fifteen NA initializer rows, including all fourth-word zeros, retain
their exact bytes and order. The declaration stays unsized under the
existing MODEL_EFFECT_COEFFICIENT_CUSTOM_EXTERN switch. No new count or
selector bound is inferred from the recognized NA table extent.

ModelEffect_GetCoefficient keeps its accepted function name and simply
returns the indexed record pointer without validation. Its declaration,
definition, owner and all typed consumers use the recovered record name;
no exported/native symbol or per-image binding is renamed.

Both preset wrappers still select the record BEFORE their guards. Their
separate slot-dependent distance expressions retain the slot<2 branch,
minimum-fifty adjustment, signed halfword narrowing, subtraction of 300,
positive-delta halving and division by 750. They are not deduplicated,
changed into saturating arithmetic or redirected through the standalone
distance helper. Direct application forwards yaw/pitch to the existing
metric setter; queueing forwards them to the relative-orbit builder.
Their guards, optional offsets, endpoint setup and call order stay exact.

The special-background animated-battle path still uses record ten,
adds 0x800 to its yaw, copies its distance into the first endpoint word
and adds 0x400 to its pitch for the third word. Existing endpoint kind,
eight-byte transport and fixed other coordinates are untouched.
Model_GetSlotCameraPresetYaw still rebuilds the CF8 alias for the selector
load, masks by 0x1F and reads only yaw. Its signed remainder and differing
nonpositive/positive slot bias remain, including unchecked indexing.

That record-name component preserved its then-existing fixtures and
numeric Lua labels, breakpoints, observations and controls byte-for-byte.
Those are historical observations; later source-only naming neither
restores retired fixtures nor claims a new trace or cloud execution.

## Runtime observation

Two traced 3-D battles (#418, interpreter CPU, setup confirmed) called
`Model_GetSlotCameraPresetYaw` from `0x8004F150` for slot 0 and
`0x8004F43C` for slot 1. Slot 0 always returned `0xA00` and slot 1
`0xE00`, in both battles and on both the default arena and the mountain
field.


## Animated-battle camera and handler phase views

The complete Model_UpdateAnimatedBattle definition now distinguishes its
captured effect_handlers array, camera_endpoint transport and
defender_handler_index. The four-word handler copy remains before the
live card-stat classification and signed phase switch. The index is still
one or two from the original slot1 model-id lookup and bit test, not a new
card-class enum, range check or handler rebinding. Module arguments and
slot/global reads remain at their original call sites.

The special-background branch retains preset ten, fixed orbit inputs,
the eight-byte endpoint copy, distance-to-X and pitch-to-Z transport,
and its separate key_magnitude. Raw current_key_halfwords views in the
two readiness phases retain null checks and fourth-halfword kind tests.
The low-five-bit option is still remapped in ONE preset_index binding:
0/1/2/3 become3/0/1/2 and larger values become3. Its key_magnitude retains
the negative initial value and conditional signed halving; no duration,
selector normalization or new shared camera helper is introduced.

The two action-start windows distinguish target_yaw, yaw_step_limit and
yaw_delta. Keep the signed delta arm, separate comparison in the other
arm, every live yaw read and (frame_step+2)*8 order. Do not replace these
with absolute-value math, shortest-angle logic or a cached view snapshot.
The two later action classifiers deliberately KEEP their mixed-role a
bindings: each holds a row classification THEN a scaled distance. Their
separate move_magnitude copies retain minimum-ten clamps and call order.

Each selected_slot_index is a phase-local boolean, not a winner/loser or
universally validated row-six slot. Earlier phases STILL use slot0 row!=6;
later phases STILL use row==6. Opposite-slot selection keeps XOR1, including
the first pointer captured before its readiness guard. All callback polls,
field_E15 stores, raw sign/tint gates, tie comparisons, stone-state gates,
timer stores/narrowing and phase publications remain in original order.
The final tint phases name current_yaw, biased_yaw, current_distance,
opposite_slot_index and move_magnitude without changing when remainders
are taken, unsigned window tests, distance scale/clamps or negative calls.
The explicit slot_base and selected_slot bindings remain distinct.

All regional branches, including the Japanese poll-only case versus the
non-Japanese diagnostic path, and the final unconditional orbit-direction
lock remain. No callback ownership, action content, outcome classification,
time unit, guard, phase transition or gameplay behavior is newly asserted.

The complete Model_GetSlotCameraPresetYaw query names its existing
slot_index definition argument, slot_offset, slot_metadata, preset, yaw
and biased_yaw. Preserve the integer slot stride, CF8 interior view,
separately rebuilt alias for the selector load, unchecked selected byte,
low-five-bit mask, signed halfword yaw and both signed remainder arms.
Public prototype labels, types, pointer casts and canonical getter identity
stay. The historical #418 observations above are not a new capture.

These are only scoped bare locals and the query's definition argument.
All existing C comments, members, types, declarations/order/lifetimes,
casts, operators, constants, labels, calls, other grouped functions,
private tables, regional macros, public headers, compiler profiles and
storage remain unchanged. Two existing semantic NOTES cells are appended;
all identities, first-five-column evidence, confidence and other rows stay.
No fixture/control/oracle/mutation change, retired-fixture revival, local
cloud execution or synthetic/generated-C/source-string behavioral harness.
