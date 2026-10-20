# Hand-slot storage and ordered selection

`DuelHandSlot.object` and `.order_marker` point to canonical `DisplayObject` pool
slots. The card constructor reached through `Duel_CreateHandCardDisplayObject` allocates key 6
from `DisplayObject_AcquireSlot`; `DuelHand_CreateSelectionOrderMarker` allocates the child sprite
from the same pool with key 1. Draw, replacement, release and selection
consumers agree on those pointer identities.

The wrapper's known resident callers are hand creation during draw-phase
entry, drawn-card insertion and hand reordering. Each publishes its result
to `gDuelHand_aSlots`. It applies the active-side marker policy
after creating the screen object; it does not publish a field-card object
or normalize the incoming position-register convention.

| Member | Slot offset | Storage |
| --- | --- | --- |
| `object` | `0x00` | `DisplayObject *G32` |
| `order_marker` | `0x04` | `DisplayObject *G32`, the selected slot's marker sprite |
| `selection_order` | `0x09` | `u8`, zero when unmarked, otherwise its one-based selection order |

The hand-slot stride remains `0x0C`; five slots occupy `0x3C` bytes. This is
one active-side presentation cache, not two side arrays, a `DuelCardRecord`
table or the display-pool slot size `0x70`. Phase entry binds the existing
cache to the active selection record's `hand` pointer. The card-rendering and hand-card views remain explicit at consumers
that use their context-specific fields. The card constructor's measured
ambient position arguments and narrow return view are not normalized.

The child-stack helper formerly indexed the primary pointer as bytes to read
offset `0x16`. Its canonical member is signed, so the replacement reads
`(u8)object->field_16` before adding one and narrowing to `s8`. This retains
the unsigned byte interpretation and wrap behavior for all 256 bit patterns,
including 0x7F and 0xFF. Count updates and the `selection_order` byte still wrap as
before; no new count or allocator-safety guarantee is introduced.

The Exodia byte cursor retains its measured traversal and accesses the stored
pointer through `DisplayObject *G32 *`. Equal-sized `DisplayLinkEntry`
records are not merged into the hand-slot type solely by shape.

## Selection helpers

Both adjacent cursor/marker helpers now take the actual
`DuelSelectionRecord *` held by their hand-scene caller. The former
`DuelHandStackState` was only a private `0x18` prefix view; removing it does
not resize the real `0x1C` records or their four-record side blocks.
The helpers use `cursor_object` at `+0x04`, the signed view of `field_0E`
at `+0x0E`, and the hand-context ordered-pick count `field_15` at `+0x15`.
The shared layout asserts all three offsets, and every caller now passes
its existing record directly instead of casting to the prefix.

`field_0E` remains unsigned storage in the canonical record because other
consumers keep their own views. The two helpers explicitly cast it to `s8`,
preserving the original signed loads and arithmetic for every byte pattern.
The count remains unsigned and retains its original narrowing and wrap.
No broader meaning is assigned to `field_15` in the other selection records.

`DuelHand_PositionSelectionCursor` (`0x8001B780`) writes the existing cursor's
output position from the signed slot index and regional fixed y coordinate.
Hand actions call it after choosing or changing that index.

`DuelHand_CreateSelectionOrderMarker` (`0x8001B7AC`) lifts the current card,
creates its child sprite, then publishes the incremented running count in
`selection_order`. The controller later collects primary card objects in ascending
order-byte order; removal lowers the card, releases the child and renumbers
remaining order bytes and child cells. The helper does not own those removal
or eligibility rules, does not reuse an existing child, and does not add count
bounds. The name follows that data flow without guessing resource glyphs.

`DuelHand_RefreshSelectionColors` (`0x8001B8B8`, formerly
`func_8001B8B8`) dims **only unmarked slots**. When the hand record's
ordered-pick count is zero, it also restores the signed-indexed cursor card
to the normal color. Selected slots otherwise retain their current colors.
This is not an all-card color reset, does not create markers or change
orders, and adds no null/object-availability check.

The complete order lifecycle is shared across the existing paths: draw and
AI replacement publish the primary object, marker creation publishes the
incremented one-based order, cancellation releases that marker and
renumbers later order bytes and glyph cells, and ordered play consumes the
primary objects in order. Placement releases the marker and clears the
stored object handles while leaving the order byte unchanged until the
existing reset. A positive byte therefore does not promise a live marker
in every later phase.

`Duel_ClearHandSlots` retains its exact raw traversal and store order:
clear the `+0x04` pointer, clear the `+0x00` pointer, then zero byte `+0x09`
for each of the five `0x0C` records. It is not a whole-record clear; byte
`+0x08` and padding are not newly initialized. There is no new count bound,
allocation guarantee, ownership transfer or slot-storage definition.

The canonical argument migration preserves all sixteen complete compiler
and MASPSX artifacts for the eight affected lifecycle units raw-identically.
The subsequent identity migration compares those complete units after
reversing only the color-function name, cache name and color source's
`.file` path. Integration also retains the accepted cursor source/header
rename; its exact `.file` path change is accounted for in the complete-unit
comparison, without filtering any instructions or relocation structure.
Original grouping and profiles remain fixed: the cursor and
marker still share their `gcc_2_8_1_g8_split` object, and the color function
keeps its separate object and all native bindings. No new fixture, test
scenario or runtime trace is needed to claim these static contracts.

| Binding | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| `DuelHand_RefreshSelectionColors` | `0x8001B8B8` | `0x8001B718` | `0x8001B828` | `0x8001B82C` |
| `gDuelHand_aSlots` | `0x800EA030` | `0x800E9F10` | `0x800EAE98` | `0x800EB2B8` |

## Hand-slot record-index map

`gDuel_abHandCardRecordIndices` (originally `D_800907CC`, at North American
`0x800907CC`) maps each side's five hand slots to live `DuelCardRecord`
indices. The initialized data in `duel_field_layout.c` contains
`0, 1, 2, 3, 4` for side 0, `15, 16, 17, 18, 19` for side 1 and two trailing
zero bytes before the separate field grid. Those twelve bytes match the
North American resident input.

Draw resolution passes the mapped value as the live-slot argument to
`Duel_SetupCardRecord`, separately from the deck cursor. Hand actions use it
for the selected live record; `Duel_ApplyAiHandSelection` uses the same map
when rebuilding a hand object. These are not deck indices, AI pick codes,
display-object addresses or the field grid.

The 12-byte definition and unsized flat reader declaration remain distinct.
All initializers, side/slot arithmetic, signed hand-slot reads, stored byte
interpretations and the AI helper's integer-added table base are unchanged.
The Japanese and base-European primary bindings and all four translated
European imports keep their original addresses. The unrelated French
SDK label `D_800907CC_French` is not this map and remains untouched.

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
