# Resident camera-buffer getter return types

Two getters in `src/game/model_slot_support.c` return existing SDK
objects, not opaque allocations:

| Getter | Return type | Existing object |
|---|---|---|
| `Model_GetCameraViewBuffer` | `GsRVIEW2 *` | `D_800F56F0` |
| `Model_GetCameraCoordinateUnit` | `GsCOORDUNIT *` | `D_800F56A0` |

The types already belong to `src/game/camera_view.h`. The view initializer
fills the reference-view fields and passes the record to `GsSetRefView2`.
It also rebuilds the coordinate unit's rotation, matrix translation,
parent and flags through the SDK layout. The coordinate unit occupies the
`0x50` bytes immediately before the view. The getter definitions simply
return those typed addresses; a `void *` result was not evidence for a
different object or allocation.

## Main-game consumers and partial views

The model-effect camera setup now keeps its returned pointer as
`GsRVIEW2 *` and uses the named reference-point members directly, removing
repeated casts from a halfword pointer. Its field math and memory access
widths are unchanged.

Two consumers deliberately retain partial views: the keyframe stepper
writes word-component triples, and the Library transfer copies the view in
two sixteen-byte blocks. The Library destination local now keeps the actual
`GsRVIEW2 *` returned by the getter, rather than discarding that type through
a byte alias. Its two explicit `LibraryViewQuad` copy views remain unchanged:
the first starts at `vpx`, the second at `vry`, with no whole-struct copy or
new quad-array ownership claim. The getter still runs after the orbit update
and before both copies and the metrics update. Supported-compiler controls
check the target's two-quad extent and both copy boundaries. The explicit
partial views preserve the existing capture, aliasing and copy shapes.

Every existing overlay camera-view caller already holds a `GsRVIEW2 *`.
The return-type correction is compatible without editing overlay code.
The coordinate-unit getter has no observed static consumer. The light
matrix and view-metrics getters remain outside this bounded change.

No storage declaration, initializer, profile, address, section or object
order changes. The native keyframe fixture keeps its original returned
address and oracle under the corrected SDK pointer type. Supported-GCC
owner controls accept both typed getter contracts and reject their former
opaque function-pointer returns; full executable matching remains the
code-generation gate.
