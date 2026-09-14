# Debug menu entry map

The completed `debug_menu_entry_labels` PCSX-Redux trace observed every cursor
position on both debug-menu pages without activating an entry. The primary
page displays two columns of ten labels:

| Cursor | Label | Step handler |
|---:|---|---|
| 0 | `3D` | `DebugMenu_EnterMappedMode` |
| 1 | `Campaign` | `DebugMenu_UpdateCampaignEntry` |
| 2 | `DUEL` | `DebugMenu_EnterDuel` |
| 3 | `Detail` | `DebugMenu_EnterMappedMode` |
| 4 | `Sound` | `DebugMenu_UpdateSoundEntry` |
| 5 | `BustUp` | `DebugMenu_UpdateBustUpEntry` |
| 6 | `3D MAP` | `DebugMenu_EnterMappedMode` |
| 7 | `DeckEdit` | `DebugMenu_EnterDeckEditor` |
| 8 | `FreeDUEL` | `DebugMenu_EnterMappedMode` |
| 9 | `TITLE` | `DebugMenu_UpdateTitleEntry` |
| 10 | `NAME` | `DebugMenu_EnterMappedMode` |
| 11 | `Password` | `DebugMenu_EnterMappedMode` |
| 12 | `MOVIE` | `DebugMenu_UpdateMovieEntry` |
| 13 | `Load` | `DebugMenu_UpdateLoadEntry` |
| 14 | `Save` | `DebugMenu_UpdateSaveEntry` |
| 15 | `Trade` | `DebugMenu_UpdateTradeEntry` |
| 16 | `Option` | `DebugMenu_EnterMappedMode` |
| 17 | `LOSE` | `DebugMenu_EnterMappedMode` |
| 18 | `HIRATA's` | `DebugMenu_EnterMappedMode` |
| 19 | `EXIT` | `DebugMenu_Exit` |

`DebugMenu_Update` stores `gDebugMenu_bCursor + 1` in the shared step byte.
Consequently, element zero of `gDebugMenu_apfnPrimaryPageSteps` is the reset
handler and elements 1 through 20 correspond to cursor positions 0 through
19. The generic entries index `gDebugMenu_abMainModeByEntry`; dedicated
handlers ignore the corresponding byte in that table.

Pressing Select toggles `gDebugMenu_bPage`. The alternate page visibly labels
only cursor zero as `2PDUEL`, mapped to
`DebugMenu_UpdateTwoPlayerDuelEntry`. The green cursor can still visit all
twenty positions. Its two-element table is deliberately adjacent to the
primary table: cursor one reaches the primary table's reset entry and later
blank positions continue through the primary handlers. The trace establishes
those stored pointers and visible labels, not that activating blank positions
is supported.

The dedicated names are also consistent with their matching bodies: the sound
entry edits and plays sound IDs, the campaign entry switches between message
and campaign IDs, the duel and deck entries initialize those modes, the load,
save, and trade entries poll their existing state machines, and EXIT fades and
longjmps out. Option instead uses the generic mapped-mode handoff: it clears
the return/debug state and publishes main mode 11. No stronger claim about
hidden debug functionality is made.
