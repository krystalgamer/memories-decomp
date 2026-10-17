# SDK view input for cached camera metrics

`Model_UpdateViewMetrics` takes `GsRVIEW2 *`, not an unstructured byte
pointer. Its existing body establishes that SDK layout independently:
it copies a whole `GsRVIEW2` into the shared view when given a different
view, then reads the viewpoint and reference-point members to derive the
cached distance/yaw/pitch.

The owner header includes `camera_view.h` for the actual SDK declaration
and prerequisite headers. No guessed forward declaration, replacement
layout, or SDK header change is introduced.

## Preserved input behavior

Null still selects the shared view at `D_800F56F0`. Passing that same view
does not install a second copy; a different view is copied before metrics
are calculated. The parameter and the existing local shared-view pointer
now share the SDK type, removing only redundant byte/record casts.

Every measured `*(s32 *)&view->member` read remains unchanged, as do the
distance scratch value, full-distance publication, angle normalization,
copy order, and existing null/identity conditions. The six-byte cached
prefix, separate scalar labels, and independent eight-byte snapshot copy
are untouched.

All five static call sites pass null, and none belongs to an overlay.
The two native controller/keyframe stubs use the same SDK input type
while recording exactly the same callback kind, arguments and mutations.
Their independent oracles are unchanged.

## Contract and boundaries

Supported-GCC controls include only the public owner header, accept the
`GsRVIEW2 *` function-pointer contract plus null/non-null calls, and reject
both the former `u8 *` contract and an opaque `void *` contract. Existing
getter and cached-field controls remain in place.

No address, storage, section, source grouping, compiler profile, formula,
callback sequence, or overlay implementation changes. The separate
coordinate-builder helper retains its existing interface. The deliberate
integer-address current-key getter and its ownership controls are not
changed by this SDK input recovery.
