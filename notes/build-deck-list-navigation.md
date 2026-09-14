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
