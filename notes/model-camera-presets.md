# Model camera orbit presets

## Direct and preset application

`Model_SetCameraOrbit` is the former `func_8005F3B8`. The existing live-view
and metric getters still run before the two-byte effect-state guard.
When permitted, a nonnegative slot selects the reference point through
`Model_CopySlotU16Values`; a negative slot retains the current point.
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
Only the obsolete Japanese preset-wrapper alias retires. The existing
intro-controller control file changes one identifier, preserving all
values, assertions, stubs and execution paths.

## Coefficient, distance and yaw queries

`ModelEffect_GetCoefficient` retains the canonical name already accepted
for the Japanese getter and replaces the NA `func_8005F1A4` spelling. It
still returns the indexed `ModelEffectCoefficient *` without a new bound.
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
