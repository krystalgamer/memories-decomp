# Resident ground-position projection helpers

These names follow the North American matching sources and their resident
constructors/callers. They do not rely on reference-project types or
unverified names for the shared display-object fields.

| Original address | Name | Role |
| --- | --- | --- |
| `0x80015D18` | `DisplayObject_ProjectGroundPosition` | Project one object's ground-plane position into its screen-position pair |
| `0x80015DB8` | `DisplayObject_ProjectLinkedGroundPosition` | Copy a linked pool object's ground position, then project it |
| `0x80015DFC` | `Duel_DrawProjectedFieldCardFrame` | Project a field card's ground position and draw its card frame |

## Ground-position update callback

`DisplayObject_ProjectGroundPosition` builds a scratchpad `SVECTOR` from
the two halfwords of `object->position`: the first becomes x, the second
becomes z, and y is explicitly zero. It selects
`MODEL_DEFAULT_PROJECTION`, sets the geometry origin to `(160, 108)`, and
projects through `D_800FE148` using the official GTE RTPS macros.
The resulting screen pair is written to `field_30` and biased by
`(-32, -30)`. The geometry origin is then restored to zero.

The constructor `func_800234E4` in `duel_field_display_objects.c` supplies
the two position halves from the duel-field coordinate table and installs
this function in the display object's `update` slot. The name therefore
describes a ground-to-screen position update, not a draw routine or an
unrestricted three-axis projection.

## Linked-position wrapper

`DisplayObject_ProjectLinkedGroundPosition` first copies the complete
position word from `D_800EFE48[object->field_6A]`, then invokes the projector
above. `Duel_GetOrCreateActionReadyMarker` supplies the link by storing
`source->field_0A` in `field_6A` when it creates the follower object and
installs this wrapper as its update callback.

This establishes a linked pool-object index for this constructor/consumer
pair only. It does not rename `field_6A` globally: the same offset has other
meanings on other object kinds, including the card-frame renderer.

### Action-ready marker acquisition (`0x8001D518`)

At a selection refresh, `DuelScene_UpdateFieldActions` requests this marker
only for row 2 when `DuelCard_CanActThisTurn` reports an occupied card that
has not been used this turn. Otherwise that path releases the cached object.
It passes the selection cursor object as the projection source.

`Duel_GetOrCreateActionReadyMarker` returns `D_8009B1F8` unchanged when the
cache is populated. Only creation binds the source pool index and installs
the linked-ground projection callback. The function does not check card
eligibility or write the global cache: its caller publishes the returned
pointer. The name describes this observed action-readiness role, not a
guessed arrow, icon shape or resource-table meaning.

## Projected field-card frame

`Duel_DrawFieldCards` selects `Duel_DrawProjectedFieldCardFrame` for occupied
field records carrying flag `0x400`. Other occupied cards take the separate
`Duel_RenderFieldCard3D` path.

This function projects the pointed-to object's x/z halfwords at `0x30`
and `0x34`, again with y fixed at zero. It stores `screen_x = x - 26` and
normally `screen_y = y - 30`; the side and signed byte at record offset
`0x18` select a one-pixel y adjustment. It then calls `func_80016784`,
whose existing `func_80016784.h` contract identifies the card-frame draw
and the ordering-table argument, using the existing interior table alias.
Unlike the general ground-position callback, this path relies on the
caller's projection setup and finishes with geometry origin `(160, 108)`.

## Unchanged contracts

The source paths retain their original address-based names. Regional
inventory names and linker symbol lists follow the shared implementations,
but addresses, sizes, compiler profiles, scratchpad qualifiers, GTE macros,
callback casts and the projected-record layout remain unchanged.
The projected-frame path now takes the shared `DuelCardRecord` directly and
stores its checked `screen_x`/`screen_y` members. Its selector comparisons
still cast the unsigned `table_index` byte to `s8`; this preserves the signed
retail reads without changing the storage interpretation for other users.
The volatile second object-pointer reload is retained.
