# Camera duration from a magnitude request

`Model_SetCameraMoveDurationFromMagnitude`, originally `func_8005A074`
at `0x8005A074`, sets the existing camera move's duration and resets
elapsed progress. No static production caller is observed; the name follows
the body and the controller's advancement/interpolation/completion reads.

## Conversion and state boundary

The original expression remains exactly:

```c
camera_move->duration = (magnitude_request < 0 ? -magnitude_request : magnitude_request) * 2;
camera_move->elapsed = 0;
```

The public `s32` request is not replaced with the destination's halfword
type. Signed arithmetic, conditional negation, doubling, narrowing and
store order remain intact. `FromMagnitude` distinguishes this conversion
from an unscaled duration assignment without guessing physical time units.

The first state getter is unconditional and the second remains
short-circuited. Agreement at `1` suppresses both writes. No selectors,
flags, mode or current view are changed, and the helper does not itself
start a move or report success.

Zero and oversized requests do not acquire validation, saturation or
new overflow guarantees. In particular, this is not the separate camera
setup path's sentinel/clamping logic, and it does not establish that
every duration writer prevents a zero interpolation divisor.

## Ownership and regional identity

The unused helper retains the existing project pattern of no public
owner declaration. Strict old-GCC controls inspect the actual production
definition rather than introducing a guessed forwarding prototype.
Existing interface controls remain unchanged.

The seven-function `model_effect_requests.c` object and
`gcc_2_8_1_g0_split` profile retain the `0x68` extent:

| Build | Address |
|---|---|
| North America | `0x8005A074` |
| Japan | `0x8005CEA4` |
| European base | `0x8005E538` |
| French, German, Italian, Spanish | `0x8005D17C` |

Every region, Japan included, exports the shared
`Model_SetCameraMoveDurationFromMagnitude` name at its own address; the
Japanese adapter no longer aliases it to an address name.

No source move, storage/layout, selector meaning, runtime guard,
SDK/overlay implementation, guest-pointer cast or compiler flag changes.
