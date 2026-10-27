# Model camera reference-point interface

## Accepted orbit dependencies

The accepted `Model_StepViewOrbit` and `Model_LockOrbitDirection` names,
semantic source/header paths, profiles and declaration contracts are
retained rather than renamed again. The four/five-argument
`FUNC_80058434_CALL_WITH_UNUSED_ARG` selector belongs to the orbit step;
the lock retains its separate `int` signature and ordered guard.
The existing camera/keyframe controls remain byte-for-byte accepted.

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

Only scoped bare locals in these FOUR complete definitions change.
Arguments, member tokens, comments, types, constants, expressions,
scopes, labels and order remain. All other grouped definitions,
headers, SDK declarations, callers, fixtures and controls stay exact.
The existing extent and slot-refresh extractors select other bodies;
the intro witness stubs the direct orbit setter. No new harness or
local cloud-witness execution.
