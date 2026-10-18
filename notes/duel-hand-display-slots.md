# Hand display-slot pointers

`DuelHandSlot.object` and `.child` point to canonical `DisplayObject` pool
slots. The card constructor reached through `Duel_CreateHandCardDisplayObject` allocates key 6
from `DisplayObject_AcquireSlot`; `DuelHand_CreateSelectionOrderMarker` allocates the child sprite
from the same pool with key 1. Draw, replacement, release and selection
consumers agree on those pointer identities.

The wrapper's known resident callers are hand creation during draw-phase
entry, drawn-card insertion and hand reordering. Each publishes its result
to `tent_DuelHandDisplayRecords`. It applies the active-side marker policy
after creating the screen object; it does not publish a field-card object
or normalize the incoming position-register convention.

| Member | Slot offset | Storage |
| --- | --- | --- |
| `object` | `0x00` | `DisplayObject *G32` |
| `child` | `0x04` | `DisplayObject *G32` |
| `active_09` | `0x09` | unchanged byte |

The hand-slot stride remains `0x0C`; it is not the display-pool slot size
`0x70`. The card-rendering and hand-card views remain explicit at consumers
that use their context-specific fields. The card constructor's measured
ambient position arguments and narrow return view are not normalized.

The child-stack helper formerly indexed the primary pointer as bytes to read
offset `0x16`. Its canonical member is signed, so the replacement reads
`(u8)object->field_16` before adding one and narrowing to `s8`. This retains
the unsigned byte interpretation and wrap behavior for all 256 bit patterns,
including 0x7F and 0xFF. Count updates and the `active_09` byte still wrap as
before; no new count or allocator-safety guarantee is introduced.

The Exodia byte cursor retains its measured traversal and accesses the stored
pointer through `DisplayObject *G32 *`. Equal-sized `DisplayLinkEntry`
records are not merged into the hand-slot type solely by shape.

## Selection helpers

`DuelHand_PositionSelectionCursor` (`0x8001B780`) writes the existing cursor's
output position from the signed slot index and regional fixed y coordinate.
Hand actions call it after choosing or changing that index.

`DuelHand_CreateSelectionOrderMarker` (`0x8001B7AC`) lifts the current card,
creates its child sprite, then publishes the incremented running count in
`active_09`. The controller later collects primary card objects in ascending
order-byte order; removal lowers the card, releases the child and renumbers
remaining order bytes and child cells. The helper does not own those removal
or eligibility rules, does not reuse an existing child, and does not add count
bounds. The name follows that data flow without guessing resource glyphs.
