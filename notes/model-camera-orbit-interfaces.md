# Model camera reference-point interface

## Accepted orbit dependencies

The accepted `Model_StepViewOrbit` and `Model_LockOrbitDirection` names,
semantic source/header paths, profiles and declaration contracts are
retained rather than renamed again. The four/five-argument
`FUNC_80058434_CALL_WITH_UNUSED_ARG` selector belongs to the orbit step;
the lock retains its separate `int` signature and ordered guard.
The earlier alias component preserved its then-existing camera/keyframe
controls byte-for-byte; that is historical evidence, not a request to
restore retired gameplay fixtures.

This component changes neither operation. Its only change in the accepted
orbit-step body is the reference-point alias identity below; signed
arithmetic, pivot selection, call order and casts remain exact.

## Reference-point interior view

`gModel_ViewReferencePoint` is the former `D_800F56FC`, the alias beginning
at `gModel_RView + 0x0C`, where the SDK `GsRVIEW2` stores `vrx`, `vry` and
`vrz`. Its declaration moves from `unmatched.h` to `camera_view.h` without
changing the incomplete `s32 []` type or introducing a standalone vector.
The orbit step retains its explicit `PSXLONG *` cast and three-word
access rather than replacing the alias with typed field indexing.

All seven native images retain the same interior offset:

| Identity | NA | Japan | UK | Continental Europe |
|---|---|---|---|---|
| Live view | `0x800F56F0` | `0x800F2F90` | `0x800F6820` | `0x800F6C28` |
| Reference point | `0x800F56FC` | `0x800F2F9C` | `0x800F682C` | `0x800F6C34` |

The live view's existing halfword/`GsRVIEW2` declaration arms, saved copy,
coordinate unit, native binding order and surrounding bytes are unchanged.

## Preset distance and direct-orbit local views

The standalone distance adjustment, direct preset wrapper, direct
orbit setter and queued preset wrapper now name only existing locals.
dimensions and size_delta retain the four-s16 adjustment record,
minimum50, subtraction300 and signed-halfword store/reload. The helper's
slot>=2 passthrough and wrappers' slot<2 branches stay SIGNED, including
their original negative-slot behavior. No new dimension bound or exact
mesh-size interpretation is added.

All THREE adjustment implementations remain independent. distance_basis
is full distance unless positive size_delta selects half distance.
The helper retains unsigned sign-bit extraction before its signed
shift; the wrappers retain signed /2. Keep divisor750, product/division
order, all narrowing and every callback. The direct wrapper's
distance_work remains ONE binding for full/half distance THEN product;
size_product stays separate. No helper extraction or algebraic rewrite.

Both preset pointers are selected BEFORE their respective state guards.
distance captures the signed preset halfword before the optional
dimension callback; yaw/pitch are STILL read later for forwarding.
Do not prefetch the whole preset, cache away guard reads or merge the
different nested/short-circuit guard expressions. The fifteen existing
records, unsized declaration, signed halfword fields and opaque field_06
stay exact; no new selector bound, mask-derived count or table layout.

The direct setter obtains view THEN metrics BEFORE its state guard,
retaining those calls even when the guarded path returns. Nonnegative
mode copies FOUR halfwords to slot_position but publishes only xyz;
negative mode keeps the existing reference point BEFORE any offset.
The existing short[4] representation stays. offset_sign affects X/Z
only, with mode<=0 selecting+1; Y is added directly. No signedness,
slot validation or offset ownership change.

Keep distance/yaw/pitch publication order, signed remainders and
nonpositive/positive side bias. The exact FIVE-argument orbit call
retains full-turn yaw and trailing1. That full-turn value bypasses the
MASKED yaw-update test but satisfies the RAW nonzero recomputation
test, rebuilding from the newly published metrics. It is not replaced
by zero. Pose capture with1 STILL follows, then camera request-1.
No immediate SDK view installation or new angle normalization promise.

That earlier FOUR-definition component changed only scoped bare locals.
Arguments, members, comments, types, constants, expressions, scopes, labels
and order were preserved. Its then-existing extent/slot-refresh extractors
selected other bodies, and its intro witness stubbed the direct setter.
Those are historical fixture observations; the later input-view component
below neither restores nor executes retired gameplay witnesses.


## Complete direct and queued orbit input views

FIVE complete definitions now name their existing input labels:
slot_selector, requested_distance/distance, preset_index, yaw/pitch,
target_offset and the queued key_magnitude. These are definition labels
ONLY; public prototype labels, all signatures and int/s32 views remain
byte-identical. slot_selector is still SIGNED and unchecked: the distance
helper's >=2 passthrough, wrappers' <2 adjustment, direct setter's >=0
copy and <=0 side sign, and queue builder's EXACT2 special case differ.
No shared selector/guard helper, range repair or negative-input policy.

The three size-adjustment implementations retain their independent
expressions, halfword clamp/subtract/reload and arithmetic order.
The distance helper STILL extracts the unsigned sign bit before signed
halving shift; the two wrappers STILL use /2. Keep the direct wrapper's
separate size_product and full/half-then-product distance_work binding.
Preset pointers remain selected before guards, distance remains captured
before the dimension helper and yaw/pitch remain later member reads.
The queued wrapper's dimensions is only its existing local adjustment
record, not new storage or a new interpretation of mesh bounds.

camera_view and view_metrics retain getter order BEFORE the direct guard.
Its short[4] slot_position copy still publishes xyz only; negative selectors
retain the old pivot before the optional offset. Side-dependent signs
remain X/Z-only. Keep u16 metric stores, signed remainder forms and exact
orbit-step/full-turn/trailing1, pose-capture1, request-1 call order. The
queued path does NOT become an immediate view installation.

The complete queue builder distinguishes raw u16 target_halfwords[4] from
orbit_halfwords[4]. The former is zeroed before the guard and receives the
original selector|0x80 halfword, or FOUR slot2 halfwords followed by kind1.
The latter receives distance and the original angle deltas, then kind4.
They remain aligned raw arrays, not packed endpoint locals. The final
call STILL passes orbit FIRST and target SECOND through the old casts.
key_magnitude is forwarded unchanged to the existing signed key-magnitude
publication; it is not guaranteed to be positive, frames or a fixed duration.

view_metrics is captured by the original getter before target initialization
and the guard. offset_sign affects X/Z; the middle offset remains an explicit
u16 read, unlike the direct setter. biased_yaw/biased_pitch retain
full-turn addition and their original signed quotient/remainder forms.
yaw_word keeps the zero-extended halfword snapshot before its correction.
Yaw still explicitly narrows the live metric to s16; pitch still reads its
u16 metric directly. Preserve all intermediate halfword stores, threshold
tests, wrap adjustments and cast-based signed comparisons. No canonical
angle, shortest-delta-for-all-inputs, wider arithmetic or bounds promise.

The shared x scratch STILL holds adjusted yaw THEN the signed batch ordinal;
it is deliberately not split or given one false single-phase meaning.
queue_mode begins with the original signed-byte ordinal and keeps the
separate x copy, negative-to-zero branch, later live global increment and
>0 classification. The constant-one binding, gate expressions and all
captured-versus-live reads remain. No batching, hold-state or callback
normalization is introduced.

Only scoped bare locals/definition arguments across these FIVE definitions
change. All existing comments, public headers, other functions, members,
types, declarations/order/lifetimes, casts, constants, expressions, labels,
calls, macros, table/private data, profiles and storage remain. Five existing
CSV NOTES cells are appended; all identities/evidence/confidence and other
rows stay. No fixture/control/oracle/mutation changes, synthetic/generated-
C/source-string behavioral harness, retired fixture revival or local cloud
execution.
