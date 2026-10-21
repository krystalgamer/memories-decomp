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
