# Camera move completion query

`Model_IsCameraMoveComplete`, originally `func_8005A878` at `0x8005A878`,
is the resident intro's camera-move completion gate. It reports move
control state, not a physically stationary camera.

The existing predicate remains:

```c
flags == 0 && (arg != 0 || mode == 0 || follow_countdown == 0)
```

Here `follow_countdown` describes the existing `field_02` value without
renaming it. A nonzero argument bypasses that additional check. All
**twelve** observed static intro calls pass `1`, so their test reduces to
zero interpolation flags; no observed zero-argument scene is inferred.
The earlier owner's count of eleven is corrected.

## Distinct control systems

Follow/orbit tracking can continue after interpolation flags clear.
The effect-key queue has its own empty predicate and is explicitly tested
separately at several intro sites. The query also does not inspect the
two state bytes that guard camera requests. Completion therefore does not
promise stillness, an empty key queue, or permission to issue a request.

The original `int` input/result, zero-or-one result and every flag/mode/
counter branch remain intact. No Boolean conversion, branch simplification,
new range/zero-divisor validation or side effect is added.

## Source ownership and regional identity

The standalone source/header are `model_is_camera_move_complete.c/.h`.
Every caller include, all matching manifests and tracked split paths
follow that source move. The profile remains `gcc_2_8_1_g0_split`, the
extent `0x4C`:

| Build | Address |
|---|---|
| North America | `0x8005A878` |
| Japan | `0x8005D6A0` |
| European base | `0x8005ED3C` |
| French, German, Italian, Spanish | `0x8005D980` |

No SDK/overlay implementation, stored layout, alias, selector, callback
order, integer-address API or compiler setting changes. Whole-source and
caller comparisons plus exact executable matching preserve the unused
argument-zero path as well as current caller behavior.
