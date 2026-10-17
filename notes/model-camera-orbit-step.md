# Guarded camera orbit-step request

`Model_SetCameraOrbitStep`, originally `func_8005A130` at `0x8005A130`,
sets the current orbit step and, where present, its baseline. No static
production caller is observed; the role follows the stores and the full
consumer chain.

The controller multiplies `field_04` by frame step and the direction byte,
then supplies the result as `func_80058434`'s yaw-delta argument. That
callee adds the argument to cached yaw and rebuilds the view. This is
orbit-step evidence, not an inference from the field's offset alone.

## Regional baseline behavior

The non-Japanese setter writes baseline `field_06` before current
`field_04`. NA adjustment raises the current value toward `0x10` and
otherwise lowers it toward baseline by one. Europe uses increments of two
and a `0x200` upper bound, retaining its baseline floor behavior.

Japan has a compact record without `field_06`; its setter only writes
`field_04`, and adjustment does not perform those current/baseline steps.
Neither the absent field nor non-Japanese behavior is introduced there.

## Exact request boundary

The nested state-getter guard, full-width signed input, original halfword
narrowing and store order remain unchanged. No clamp, range validation,
physical speed unit, direct view update or immediate move start is added.
Field/global names and all signed direction/frame-step arithmetic remain
as before.

The unused helper follows the existing no-public-declaration pattern.
Strict old-GCC controls inspect the actual definition, including its
Japanese compact variant, without adding a guessed owner prototype.

The seven-function request object and `gcc_2_8_1_g0_split` profile stay
intact:

| Build | Address | Extent |
|---|---|---|
| North America | `0x8005A130` | `0x58` |
| Japan | `0x8005CF60` | `0x50` |
| European base | `0x8005E5F4` | `0x58` |
| French, German, Italian, Spanish | `0x8005D238` | `0x58` |

Japan retains local `func_8005CF60`; only its shared adapter macro LHS
changes. All profiles, paths, addresses and layouts remain intact.
No SDK/overlay implementation, selector enum, guest-pointer annotation or
integer-address ABI changes.
