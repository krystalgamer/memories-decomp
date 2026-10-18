# Resident cached view-metric prefix

`Model_GetViewMetricsBuffer` returns a `ModelViewMetrics *` view of the
existing three cached halfwords, not an opaque pointer:

| Offset | Field | Producer evidence |
|---|---|---|
| `0x0` | `u16 distance` | Final `SquareRoot0` of all three squared viewpoint/reference deltas |
| `0x2` | `u16 yaw` | `ratan2(z delta, x delta)` followed by full-turn reduction |
| `0x4` | `u16 pitch` | `ratan2(y delta, horizontal distance)` followed by full-turn reduction |

The owner remains `src/game/model_update_view_metrics.h`. Its assertions
fix the view at six bytes, halfword alignment, and offsets `0/2/4`.
The producer uses distance as an intermediate horizontal magnitude before
publishing the full three-dimensional distance. Its measured scalar views,
separate interior labels, and update order are unchanged.

## Storage and consumer boundaries

This is only a known prefix view. It does not redefine the allocation or
replace the independent eight-byte `ModelBytes8` snapshot at `D_8009B478`
copied to `D_8009B480` by `Model_UpdateBackgroundScroll`. The fourth snapshot
halfword is not assigned a meaning or discarded.

The model-effect pose capture and camera setup now use named fields in
place of halfword indices. `func_8005D994` retains its explicit signed yaw
interpretation, `(s16)metrics->yaw`, and its unsigned pitch read. The existing
overlay consumer's explicit signed-halfword view remains compatible and
requires no overlay edit.

The existing regional distance/yaw/pitch labels remain at:

| Region | Distance / yaw / pitch |
|---|---|
| North America | `0x8009B478 / 0x8009B47A / 0x8009B47C` |
| Japan | `0x8009B378 / 0x8009B37A / 0x8009B37C` |
| Base Europe | `0x8009C3E8 / 0x8009C3EA / 0x8009C3EC` |
| Localized Europe | `0x8009C7F8 / 0x8009C7FA / 0x8009C7FC` |

No storage definition, initializer, address, compiler profile, section, or
object order changes. The light-matrix getter and camera-update input
parameter remain outside this change.

Supported-GCC controls check the getter signature, unsigned halfword field
pointers, and owner layout assertions, and reject the former opaque
function-pointer return. The existing guarded eight-byte snapshot and
native keyframe witnesses remain independent oracles. Whole-executable
matching is the final code-generation gate.
