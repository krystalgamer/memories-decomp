# Guarded camera follow countdown request

`Model_SetCameraFollowCountdown`, originally `func_8005A0DC` at
`0x8005A0DC`, sets the counter at `ModelCameraMove + 2` through its existing
signed halfword alias `D_800F2B22`. All seven regional bindings place that
alias exactly two bytes after `D_800F2B20`.

## Complete consumer evidence

The controller's active-move branch returns before this counter work.
Its follow path subtracts the live frame-step value and writes zero when
the positive remainder test fails, retaining the second getter/read on
the continuing path. `Model_IsCameraMoveComplete` also tests counter exhaustion when
its argument is zero and camera mode is nonzero.

All current static readiness callers pass `1`, bypassing that extra test;
the name does not invent an observed zero-argument scene. The resident
intro writes `0x78` through this request. Setup's projection-named initial
constant is not a projection consumer and does not determine this role.
No projection interpretation or physical time unit is asserted.

## Exact request and alias boundary

The body remains a signed `countdown_request * 2` store, not conditional absolute value
or saturation. Its inverted `hold_state != 1 || second_getter() != hold_state` guard,
short-circuit call order, narrowing and store placement are unchanged.
It does not start motion or modify duration, elapsed, selectors or flags.

The signed scalar alias and the controller's unsigned record-field view
are both retained. The later member name is follow_countdown at +2;
the global alias, stored layout and request ABI remain unchanged. No
broader alias rewrite, range check or overflow guarantee is introduced.

## Group and regional identity

The same seven-function `model_effect_requests.c` object and
`gcc_2_8_1_g0_split` profile retain the `0x54` extent:

| Build | Address |
|---|---|
| North America | `0x8005A0DC` |
| Japan | `0x8005CF0C` |
| European base | `0x8005E5A0` |
| French, German, Italian, Spanish | `0x8005D1E4` |

The owning declaration remains in `model_effect_requests.h`. Complete
implementation/caller comparisons differ only by the identifier, and
strict owner controls preserve the full-width request interface.
No source move, SDK/overlay implementation, selector enum, compiler flag,
guest-pointer annotation or integer-address ABI changes.
