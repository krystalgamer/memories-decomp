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

## Applying AI hand picks

`Duel_ApplyAiHandSelection` (`0x8001BAF0`) applies an already chosen AI
card list to the hand. Its resident call at `0x8001C030` runs after
`AiScript_Run` reports completion, before the controller walks the
resulting hand-slot codes. `AiScript_PlayFaceUp` supplies the five picks
from VM registers and writes the trailing zero.

The helper copies the current hand indices to scratch and marks existing
hand picks (`0xB` through `0xF`) unavailable there. This protects those
cards; it does not clear the actual hand. For a pool pick at `0x10` or
above, it finds an unreserved occupied hand slot, exchanges the selected
and hand card-record payloads, and rebuilds that hand object's display.
The explicit index exchange before the whole-record exchange retains each
record location's `deck_index`. It then publishes the resulting hand index
and rewrites the pick as `0xB + slot`, which the next controller state
uses to position and mark the selection cursor.

This is neither AI card choice nor a random shuffle. The zero terminator,
five-slot search and unchanged-pick behavior when no slot is available
remain. The deck base recovered below `gDuel_aActiveCards`, the one-pass
goto search, integer-added bases and new-object-before-old-release order
are preserved. The existing grouped source/header paths, source order,
compiler profiles and region-specific addresses are unchanged.
