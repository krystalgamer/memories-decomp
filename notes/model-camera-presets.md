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
Only the obsolete Japanese provider and caller preset-wrapper aliases
retire. The existing intro-controller control file changes one identifier,
preserving all values, assertions, stubs and execution paths.

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

All fixtures and numeric Lua labels, breakpoints, observations and
controls remain byte-for-byte unchanged. No fixture/cloud execution,
new harness, capture, report refresh, source regrouping or compiler flag.

## Runtime observation

Two traced 3-D battles (#418, interpreter CPU, setup confirmed) called
`Model_GetSlotCameraPresetYaw` from `0x8004F150` for slot 0 and
`0x8004F43C` for slot 1. Slot 0 always returned `0xA00` and slot 1
`0xE00`, in both battles and on both the default arena and the mountain
field.
