# Model camera transition and presentation defaults

This component names five resident data views used by the model camera
transition and presentation controllers. It changes identities, not
storage, layouts, access widths, guards or behavior.

## Transition record and interior views

`gModel_CameraMove`, formerly `D_800F2B20`, is the existing
`ModelCameraMove` record populated by `Model_StartCameraMove`, updated by
`Model_UpdateCamera` and tested by `Model_IsCameraMoveComplete`. Mode,
flags, endpoint slot selectors, elapsed/duration and interpolation
coordinates retain their existing definitions.

`gModel_wCameraFollowCountdown`, formerly `D_800F2B22`, is the record's
`+2` view. The setter stores twice its request under the existing guard;
setup initializes it to 300, follow mode decrements it toward zero and
completion may wait for it. It is not the move duration. The canonical
declaration keeps the old signed `s16` scalar even though the record member
is `u16`; neither side is retyped.

`gModel_wCameraTargetSlot`, formerly `D_800F2B4A`, is the `target.slot`
interior view consumed by distance correction. Its `s16[]` declaration
remains incomplete, not a new scalar or allocation.

| Image | Record | Countdown | Target slot | Record extent |
| --- | --- | --- | --- | --- |
| NA | `0x800F2B20` | `0x800F2B22` | `0x800F2B4A` | `0x2C` |
| Japan | `0x800F03C0` | `0x800F03C2` | `0x800F03E8` | `0x2A` |
| SLES-03947 | `0x800F3C50` | `0x800F3C52` | `0x800F3C7A` | `0x2C` |
| Other PAL images | `0x800F4058` | `0x800F405A` | `0x800F4082` | `0x2C` |

Full layouts put `target` at `+0x1C` and its slot at `+0xE`, yielding
`+0x2A`. Japan lacks `base_orbit_step`, puts `target` at `+0x1A` and therefore
uses `+0x28`. Original compact-layout selection and the full declaration
used by the Japanese completion routine's common-prefix accesses remain.

The French literal labels suffixed `_French` at the NA numeric addresses
are not these logical PAL camera bindings. They remain untouched.

## Named control halfwords

The record keeps follow_countdown u16 at +2 and orbit_step u16 at +4;
full records also keep base_orbit_step u16 at +6. Japan does not gain
that absent word. The signed countdown alias remains independent.
Setup, guarded setters, follow completion and region-specific adaptive
orbit behavior are traced in [the controller protocol](model-camera-controller.md#follow-countdown-and-adaptive-orbit-step).
Names do not establish a new range, hard cap, time unit or layout.

## Two different presentation defaults

`gModel_AnimatedBattleCameraEndpoint`, formerly `D_8009AFEC`, retains the
initialized `.sdata` `u32[2]` owner and the caller's unknown-bound view.
NA words `0x10000000/0x00040000` are halfwords `{0, 4096, 0, 4}`.
The animated-battle controller copies all eight bytes through an `SVECTOR`
transport and passes them as the first effect-key endpoint. Its fourth
halfword is active kind 4, not unused vector padding. Special-background
selection changes the three components but preserves the kind. The kind-4
evaluator orbits the eye around the opposite point and retains the current
radius when component zero is zero.

`gModel_ExodiaOrbitOffset`, formerly `D_8009AFFC`, retains the other
initialized `.sdata` `u32[2]` owner and unknown-bound caller view. NA words
`0xFA240000/0` are `{0, -1500, 0, 0}`. The Exodia controller copies this
default into its local offset and changes coordinates before calling orbit
setup/queueing. It is an orbit-center offset, not the same kind-4 endpoint
as the animated-battle default. Neither object is retyped as `SVECTOR` or
`ModelEffectEndpoint`; block-copy alignment and width remain.

| Image | Animated-battle endpoint | Exodia offset |
| --- | --- | --- |
| NA | `0x8009AFEC` | `0x8009AFFC` |
| Japan | `0x8009AF54` | `0x8009AF5C` |
| SLES-03947 | `0x8009BF74` | `0x8009BF84` |
| Other PAL images | `0x8009C37C` | `0x8009C38C` |

Original wrapper aliases and per-image root bindings establish these
providers. In particular, Japan's first default is not `0x8009AF4C`, and
the other four PAL images rebind the SLES-03947 source names to different
addresses. Only NA bytes were inspected locally: all six regional retail
inputs are absent. The general endpoint name does not assert identical
uninspected regional values or modes. Actual regional validation stays on
the PR, without local cloud/full regional/overlay runs.

## Compatibility and preservation

The isolated canonical header retains the old record type, scalar/interior
views and both incomplete/fixed-array selector contracts. All pre-existing
headers and overlay C remain unchanged. Native providers and old aliases
remain available; no new record or interior storage is allocated.
The NA record/countdown numeric providers remain in `c_symbols.ld`, retaining
the frozen central signed declaration's existing admission. Canonical aliases
refer to those providers instead of changing the validator or its exceptions.

The two existing ILP32 camera/intro witness files receive namespace-only
identifier/include migrations. Their generators, assertions, offsets,
expected widths, case counts and behavioral mutations are unchanged. No
test case, C fixture, source-string harness or probe is added. Existing
legacy ownership/type controls remain unchanged, and witness execution
is left to PR validation.

The data owner still uses the registered `gcc_2_8_1_g8` profile in
`data_c.json`; all matching manifests, profiles, function inventories and
historical attempts stay intact. Whole compiler/MASPSX comparisons cover
that owner and every actual regional caller context. Complete fallback
assembly and caller outputs are compared with only the five declared
identities and their original per-context aliases reversed, without
instruction, register, directive or filename filtering. Final clean
resident matching, ownership/policy checks and fresh PR gates remain
required.
