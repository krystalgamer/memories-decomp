# Resident effect-key preparation

`Model_PrepareEffectKey`, originally `func_8005E808` at `0x8005E808`,
prepares one effect key for the resident keyframe runtime. The
high-confidence name covers more than duration calculation.

## Ordered effects and readiness

The function first derives its magnitude scale, refreshes slot-derived
resolved endpoints through `func_8005FB30`, and resets the input key's
progress. It then tests **the global current key's readiness**, not the
input key's readiness. Those keys are separate source expressions and
must not be conflated.

If that guard permits further work, the function marks the input ready
and initializes its magnitude-based duration, retaining the `0x4000`
threshold. Negative magnitudes additionally extend timing from the
relative-position, sampled-spline or orbit-distance bounds. Both
intrinsic absolute values, unsigned arithmetic, case-local temporaries,
bound checks and original narrowing remain unchanged.

Queue setup invokes the preparer while walking queued keys. The active
updater invokes it during pending preparation and after advancing the
current cursor. This lifecycle supports `PrepareEffectKey`; a
duration-only name would omit the earlier endpoint/progress effects and
readiness behavior. The name does not imply that every call reaches all
initialization stores or that the global cursor equals the input.

## Source, adapters and contracts

The function opens the grouped `model_effect_state.c` unit, directly
before `Model_EvaluateKeyframeSpline`; it previously had its own
`model_prepare_effect_key.c` source and European and Japanese include
adapters. The unit's adapters already alias `gModel_pCurrentEffectKey`
to the same regional targets. All seven matching manifests and tracked
split paths follow the source without changing profiles, function order
or layout. The profile remains `gcc_2_8_1_g8_split`, and the extent remains
`0x3EC`:

| Build | Address |
|---|---|
| North America | `0x8005E808` |
| Japan | `0x8004E258` |
| European base | `0x8004E614` |
| French, German, Italian, Spanish | `0x8004E818` |

The existing `Key *` interface stays in `model_keyframe_spline.h`.
Implementation, callers and native witnesses change only the symbol and
necessary source locator/include paths. Complete fixture oracles, event
data, four-halfword coordinate buffers, callback state and ownership
controls are retained. Historical attempt paths remain historical.
No SDK/overlay implementation, storage, compiler flag, selector meaning
or runtime validation policy changes.
