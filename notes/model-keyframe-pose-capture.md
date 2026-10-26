# Keyframe starting-pose capture

`Model_CaptureKeyframePose`, originally `func_8005F070` at `0x8005F070`,
captures the baseline used by the resident model-effect keyframe runtime.
The name is high confidence, not a recovered original symbol.

The function always writes three current-pose channels at `gModel_aPoseSnapshot`:
the reference view's viewpoint, its reference point, and cached
distance/yaw/pitch. Each channel receives only its three component
halfwords; the `SVECTOR` pad halfwords remain untouched.
`Model_EvaluateKeyframeSpline` independently establishes the purpose by
using these channels as its initial control point before the queued keys.

The queue-start path captures this baseline before appending keys, and
`Model_UpdateEffectKeyframes` captures it when the current key completes, before advancing
the cursor. Direct camera setup calls it after updating the view.

## Optional endpoint refresh

A nonzero `capture_endpoints` flag additionally examines the current key,
if present. Requested endpoint kinds 2 and 3 supply model-slot indices;
`Model_CopySlotPosition` reads each selected slot's four halfwords into
the corresponding resolved endpoint at key offsets `0x10` and `0x18`.
This is a read from model slots, not a write back to them.

The flag does not gate the three-channel pose capture. The helper does
not evaluate the spline, advance the ring, update key timing, or replace
the separate eight-byte cached-metric snapshot.

## Wiring and preserved boundaries

The function remains in its original position within the contiguous
23-function `model_effect_state.c` object, with the existing compiler
profile and `0x104`-byte extent. Its owner declaration, three call sites,
native keyframe fixture stub and related documentation use the same name.
The fixture changes only the called symbol, not its oracle or event data.

All seven function inventories and symbol bindings retain their original
addresses:

| Build | Address |
|---|---|
| North America | `0x8005F070` |
| Japan | `0x8004EAC0` |
| European base | `0x8004EE7C` |
| French, German, Italian, Spanish | `0x8004F080` |

No source move, grouped-object split, overlay implementation, storage,
field layout, branch, arithmetic, call order, or compiler setting changes.
