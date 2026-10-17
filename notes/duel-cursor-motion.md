# Shared display object for cursor motion

`DuelSelectionRecord.cursor_object` points to the canonical `DisplayObject`,
rather than the separate `DuelFieldCursorObject` prefix view. The cursor display-object
constructor in `duel_field_display_objects.c` allocates that same pool
object, and `DuelCursor_ProcessGridDirection` only accesses its fixed members: it does not
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

## Signed field-display coordinate reads

The field text builder (`0x80023144`) and transition stepper (`0x800235C0`)
read the canonical object's +0x28/+0x2A position pair and +0x30/+0x32
output pair as signed halfwords. Their ten reads use explicit `s16` value
casts rather than rebuilding signed pointers to those already-typed members.
This preserves negative coordinates, including stored halfwords 0x8000
through 0xFFFF, without changing the shared unsigned storage or assigning
the coordinate interpretation to other object kinds.

The casts occur before the existing 8.8-fraction assembly, interpolation
arguments and text-position offsets. Fraction bytes, arithmetic order,
halfword stores, completion snaps and status handling are unchanged.

## Active selection-record pointer

`D_8009B1B4` stores a `DuelSelectionRecord *`, not a pointer to the narrower
pick-cursor view. Every resident assignment chooses a full record in the
selection table: a side base or one of its +0x1C/+0x38/+0x54 aliases, using
the existing side stride. No writer publishes a standalone 0x1A cursor.
The phase-entry hand store at +8 and the battle/Exodia cursor-object access
at +4 therefore use the canonical members without bridge casts.

Both phase-entry hand-pointer stores now use that same canonical member.
The removed `DuelSelectionSideView` only described the prefix through +8;
no code allocated, advanced or copied records using its prefix size. The
canonical hand-at-+8 and 0x1C record-size assertions remain in force.

The global remains a four-byte gp-relative pointer. The table aliases,
operand order and side-index calculations remain distinct to preserve their
relocations. `DuelCardPickCursor` still has its checked 0x1A size and
separate role at the pick-cursor API; that call uses an explicit view.
The field-display consumer also retains its phase-specific view. These are
intentional member/ABI views, not alternate declarations of the global.

## Grid-direction handlers

`DuelCursor_ProcessGridDirection` (`0x80023D08`) starts a bounded grid move
from direction bits 0/1, publishing the cursor target, velocity, duration and
movement requests. The frame integrator remains `0x800235C0`. The helper
also completes a settling move: status bit 0x80 makes it wait for the global
movement request to clear, then refresh the field text, clear status bits
0xC0 and play sound 6. That arm precedes the negative-direction check.

`DuelCursor_ProcessHeldGridDirection` (`0x80023FBC`) derives that argument
from held input, not newly pressed input. With a stable pad word, the
RIGHT/DOWN/LEFT/UP tests make UP win over LEFT, then DOWN, then RIGHT; no
direction produces -1. Each test retains its own volatile read, so this is
not a promise of a single input snapshot or a combined diagonal direction.
Both helpers return void; the existing status wrappers remain separate.

The address-based `FUNC_80023D08_AMBIENT_DIRECTION_ARG` switch and the
no-expression settling call are unchanged. Naming does not normalize either
measured argument boundary.
