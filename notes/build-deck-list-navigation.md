# Build Deck list navigation

The completed `build_deck_card_grid_cursor` trace distinguishes Build Deck's
two vertical card lists from the Library's two-dimensional grid. Build Deck
uses the two `CardList` records in `gBuildDeck_pState`; it does not reuse
`gCardGrid_bCursorColumn` or `gCardGrid_bCursorRow`.

## Observed layout

The trace started on pane zero, the 722-row chest list. DOWN and UP changed
only that list's `cursor`. R1 and L1 moved its `first_target` by eight rows,
then `first` advanced until it reached the target. The Library grid column and
row remained zero through all four inputs.

RIGHT selected pane one, moved `gGraphics_sViewportX` from 0 to 320, and
exposed the 40-row deck list. LEFT restored pane zero and viewport x zero.
Each pane retained its own `first` and `cursor` because the workspace contains
two complete `CardList` records at offsets `0x4` and `0x2D50`.

The matched runtime now reflects those roles:

| Address | Name | Role |
|---|---|---|
| `0x800330BC` | `BuildDeck_UpdateCardListInput` | Updates one list's row cursor, scroll target, paging, and sort choice |
| `0x80033500` | `BuildDeck_GetActiveCardID` | Returns the occupied card at `first + cursor` |
| `0x8003353C` | `BuildDeck_UpdateDeckPaneInput` | Handles the 40-row deck pane and LEFT transition |
| `0x800336F0` | `BuildDeck_UpdateChestPaneInput` | Handles the 722-row chest pane and RIGHT transition |
| `0x80033998` | `BuildDeck_HasOpenDeckSlot` | Tests the deck list for an unoccupied row |
| `0x8009B2FC` | `gBuildDeck_pState` | Points at the complete two-list Build Deck workspace |

`gDuel_wSelectedCardID` still has screen-dependent behavior. In the Library it
is the selected card ID; in Build Deck paging redraws make it end on the
bottom visible row's card ID. That shared global is therefore not the list's
cursor position and is not renamed by this result.

## Shared card-entry contract

The initializer, sorters, count updates, text boxes and page renderer all
operate on the same sixteen-byte `CardEntry` in `card_list_text_boxes.h`.
The former `CardListSortItem` and European `CardListRenderEntry` were partial
views of those same rows, not separate allocations or formats.

| Offset | Member | Evidence |
| --- | --- | --- |
| `0x00` | `u32 key` | Every sort mode writes it; both comparators read unsigned words |
| `0x04` | `u16 id` | Initializer, text-box and active-card consumers; sorting, count search and stat rendering retain explicit signed-halfword views |
| `0x06` | `s16 attack` | Initializer decodes the packed attack statistic; page renderer prints it |
| `0x08` | `s16 defense` | Initializer decodes defense; NTSC rendering keeps its cursor at this member |
| `0x0A` | `u8 type` | Initializer extracts the five-bit type; renderer distinguishes monster rows |
| `0x0B` | two opaque bytes | No new meaning inferred |
| `0x0D` | `u8 flags` | Initialization, sort availability, count updates, text boxes and dimmed-row rendering |
| `0x0E` | two opaque bytes | No new meaning inferred |

The layout asserts every recovered offset and the `0x10` stride. Its word
alignment follows the actual sort-key accesses; the enclosing `CardList`
and `BuildDeckTransitionState` sizes and all existing offsets stay unchanged.
The unsigned ID field does not erase the signed readers. The deck sentinel
also retains its signed member-store view: a direct assignment of `-1` to
the unsigned field makes GCC materialize `65535` instead of signed `-1`,
changing the instruction despite storing the same halfword.

`CARD_ENTRY_FROM_MEMBER` is the shared containing-record boundary for the
existing interior cursors. Deck construction and NTSC rendering walk the
defense member; chest construction walks the flags member. Their offsets
and strides now come from the shared layout, and their reads/writes name
the actual row fields rather than dereferencing hard-coded negative
offsets. These cursors and the initializer's existing arithmetic/stat local
lifetimes remain in place for exact code generation. Europe continues to
walk complete `CardEntry` records with its original stat order and nine-row
page; this does not force all regions into one induction form.

The sort API family is named together with that shared contract:

| NA address | Name | Behavior |
| --- | --- | --- |
| `0x80032B60` | `BuildDeck_CompareSortKeyDescending` | Descending unsigned key; ascending name-key tie |
| `0x80032BD4` | `BuildDeck_CompareSortKeyAscending` | Ascending unsigned key; the same ascending name-key tie |
| `0x80032C48` | `BuildDeck_SortCardList` | Builds mode-dependent row keys, sorts in place and rebuilds the visible text boxes |

The descending comparator was previously `BuildDeck_CompareCard`; the
other two were address-named. These names describe the primary key order,
not a reversal of the entire comparator. Both retain retail's return value
`1` when name keys are equal, including identical rows; no comparator-law
repair is introduced. Modes 3/4/5 select descending keys, and 1/2/6/8/9
select ascending keys. The final page rebuild remains unconditional, even
for an unhandled sort mode.

The Japanese ascending comparator is actually `0x8003293C`, between
`0x800328C8` and `0x800329B0`; it no longer needs an address-name wrapper
macro. Base Europe uses `0x80032E70/0x80032EE4/0x80032F58`, and the four
translated European builds use `0x8003305C/0x800330D0/0x80033144`.
All seven regions preserve their three addresses, sizes, profiles and
grouped-source ownership. No overlay callers are involved.
