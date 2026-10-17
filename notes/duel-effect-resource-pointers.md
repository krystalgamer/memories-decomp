# Effect-resource object pointers

The two leading stored pointers of `DuelEffectResourceRecord` refer to
`DisplayObject` pool entries, not opaque allocations. The existing paired
constructor `func_800291E0` acquires both objects with the normal pool API,
publishes the allocated-second primary in `object_00` and the secondary in
`object_04`, then links the primary from the secondary. The release helper
passes both values to the unchanged generic release API before clearing
them; the separate clear helper writes only null pointers.

Both members therefore use `struct DisplayObject *G32`, with only a forward
declaration in the record header. The `0x40` record, pointer offsets 0/4,
rectangle start at 8, source coordinates, scalar tail and every existing
offset remain unchanged. No pool allocation, null-safety or exclusive
physical-memory ownership guarantee is added.

The main-game Library consumes the primary as a display object and retains
its explicit mixed-width flag/attribute views. Other typed record consumers
use the rectangle/scalar fields. The ritual controller's separate
`D_800EA128` halfword alias accesses only the rectangle/source-coordinate
prefix beyond the two pointers; its measured accesses are unchanged.

The Password overlay stores the record pointer and sets texture source
coordinates/dimensions without accessing either object member. No overlay
implementation or public factory/release signature needs changing. Existing
constructor casts, byte views, pointer reloads and publication/release order
are deliberately retained; this change is only the stored pointee types
and target-layout/type controls.
