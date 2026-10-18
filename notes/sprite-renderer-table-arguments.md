# Sprite renderer table arguments

The second parameters of `DisplayObject_RenderSpriteStrips` and
`DisplayObject_RenderSpriteSheet` are now actual
`GsOT *ordering_table` descriptors. The old strip name `tex` was not a
texture word: its only caller selects a typed pointer from `D_800E9D90`.
The sprite-sheet list does the same for the sheet renderer.

Both list callers now pass their existing descriptor pointers directly,
and both renderer bodies pass the same pointers to the already-typed
packet dispatcher without the obsolete word-to-pointer conversion.
The respective owner headers supply the authoritative SDK type.

The **third argument stays `s32`**. The strip callee still narrows to its
existing `u16 mode16` local, and both callers still explicitly sign-extend
the object's halfword. A narrow public parameter would change a signed
`lh` into `lhu`, even though the strip callee later masks the bits.
No mode/depth type, input conversion, scratch state, loop, callback,
command-stream update, first object view, operation order or constant
changes. The secondary-callback and card-wrapper APIs are not part of
this recovery.

Supported-compiler controls accept the descriptor and full-word third
parameter and reject the old integer carrier, table-entry pointer and
narrow third parameter. The native witness executes only the two actual
caller statements with observing renderer stubs: 524,288 sequences per
`-O0`/`-O2`, covering four descriptors, two real display objects and every
halfword depth. It checks exact pointer/object identity, full signed
32-bit depth, call count and every input/guard byte. Negatives reject
unsigned depth, wrong descriptor and wrong object.

This does not execute rendering, list/update/callback logic, GTE, GPU,
SDK runtime or engine. Existing dispatcher/packet/origin/caller controls
retain all original numerical/event/type/mutation oracles. Complete
callee/list compiler and MASPSX baselines and full-executable matching
remain the acceptance gates; regional checks remain on the PR.
