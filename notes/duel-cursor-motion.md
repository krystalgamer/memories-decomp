# Shared display object for cursor motion

`GridCursor.object` now points to the canonical `DisplayObject`, rather than
the separate `DuelFieldCursorObject` prefix view. The cursor display-object
constructor in `duel_field_display_objects.c` allocates that same pool
object, and `func_80023D08` only accesses its fixed members: it does not
allocate, copy by the private type's size, or advance a private-type array.

| Former member | Offset | Canonical member |
| --- | --- | --- |
| `x` | `0x28` | `position.h.field_28` |
| `y` | `0x2A` | `position.h.field_2A` |
| `target_x` | `0x2C` | `field_2C.h.field_2C` |
| `target_y` | `0x2E` | `field_2C.h.field_2E` |
| `step_x` | `0x36` | `field_34.h.field_36` |
| `step_y` | `0x3A` | `field_38.h.field_3A` |
| `steps` | `0x60` | `field_60` |
| `moving` | `0x6C` | `field_6C` |

The removed view was `0x6E` bytes because its last byte was followed by
halfword alignment. That was not the pool allocation size: the canonical
slot remains `0x70`, as checked by `display_object.h`. All eight member
offsets and widths agree; local assertions retain the motion offset checks.

Current and target coordinates are signed reads in this consumer. Explicit
`s16` casts preserve that interpretation where the canonical union exposes
an unsigned halfword. The step and countdown assignments retain their low
halfword stores, and the moving latch remains a byte. These meanings belong
to the cursor-motion path, not every object kind that reuses the fields.

The outer `GridCursor` layout, row bounds, page, status flags and public
function declarations are unchanged. In particular, the ambient direction
arm in `func_80023D08.h` and the ambient-source call to `func_8002348C` are
not normalized. The ownership witness checks that the stored pointer is
`DisplayObject *` and that both one-argument and two-argument declaration
arms still accept only their measured function-pointer signatures.
