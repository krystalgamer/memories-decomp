# Shared display object for cursor motion

`DuelSelectionRecord.cursor_object` points to the canonical `DisplayObject`,
rather than the separate `DuelFieldCursorObject` prefix view. The cursor display-object
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

The former padded `GridCursor` view is replaced by `DuelSelectionRecord` in
the direction step and held-input forwarder. The field-display constructor
stores the cursor object at +4 in the same backing record; its +0x0F/+0x10
coordinates are the selection record's column and row, and the status
wrappers return its byte at +0x19. Neither helper allocates a record or
advances a private-view array. Seven local offset assertions and the
canonical record's existing 0x1C size assertion retain the layout checks.

| Former cursor member | Offset | Selection record member |
| --- | --- | --- |
| `object` | `0x04` | `cursor_object` |
| `col` / `row` | `0x0F` / `0x10` | `col` / `row` |
| `min_row` / `max_row` | `0x11` / `0x12` | `(s8)field_11` / `(s8)field_12` |
| `page` | `0x18` | `field_18` |
| `flags` | `0x19` | `status` |

The bounds remain signed reads, including bytes 0x80 through 0xFF. Their
storage and the page selector keep their existing neutral canonical names;
the cursor consumer's roles are not asserted for every selection phase.
The status bits and wrapper return widths are unchanged. The narrower
`DuelCursorStatus` wrapper view remains intentional.

In particular, the ambient direction
arm in `func_80023D08.h` and the ambient-source call to `func_8002348C` are
not normalized. The ownership witness checks that the stored pointer is
`DisplayObject *` and that both one-argument and two-argument declaration
arms still accept only their measured function-pointer signatures.
