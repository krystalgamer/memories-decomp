# Paired save and trade workspace

## Layout

`gSaveData_aPairWorkspace` (0x801D1200, former `D_801D1200`) is declared
in `src/game/save_pair_workspace.h` as `SaveDataPairSlot[2]`: two
`TWO_PLAYER_SAVE_SLOT_STRIDE` (`0x1000`) byte slots, one per player.

| Offset in slot | Member | Role |
|---|---|---|
| `0x000` | `loaded` (`SaveDataStateBlock`) | State block read from that player's memory card |
| `0x680` | `trade_write` (`SaveDataStateBlock`) | Trade staging block sealed and written back |
| `0xD00` | `unused_D00` | No known reader |

Three interior addresses used to have their own linker names. Nothing
in the executable tells them apart from the slot members they fall on,
so the C now reaches them through those members and the names are gone:

| Address | Former names | Member |
|---|---|---|
| `0x801D2200` | `D_801D2200`, `gSaveData_aSecondPlayerState` | `gSaveData_aPairWorkspace[1].loaded` |
| `0x801D160C` | `D_801D160C`, `gSaveData_aFirstPlayerNameSjis` | `gSaveData_aPairWorkspace[0].loaded.state.player_name_sjis` |
| `0x801D1880` | `tent_TradeSaveBuffer`, `gSaveData_aTradeWriteStaging` | `gSaveData_aPairWorkspace[0].trade_write` |

All seven regions still match byte for byte with the member spellings.

## Load and duel consumers

`SaveData_UpdateLoadPair` requests `SAVE_DATA_STATE_SIZE` (`0x680`) bytes
into slot 0's `loaded`, then into slot 1's, and compares the two duelist
codes. `SaveData_UpdateDuelLoad` requires `DECK_SIZE` nonzero halfwords
on each side before converting both `player_name_sjis` sources with
`Text_SjisToGlyphCodes`; the glyph outputs are separate views documented
in [player-name-glyph-views.md](player-name-glyph-views.md).
`Duel_InitScene` uses the two loaded states as the sides' save data on
its negative-opponent two-player path.

## Trade staging

`MainMenu_RefreshTradeInventory` lists a slot's `card_quantities`.
`MainMenu_UpdateTradeScreen` copies the first
`TWO_PLAYER_SAVE_TRANSFER_SIZE` (`0x400`) bytes of each `loaded` block to
`trade_write`, moves card counts between the two `trade_write` blocks and
calls `SaveData_RequestTradeWrite`. That function seals both
`trade_write` blocks (primary/secondary integrity), publishes slot 1's
block as the secondary transfer cursor and requests dialog step `4` with
`0x400` bytes. After successful I/O the screen copies `trade_write` back
over `loaded`. The screen still walks the slots through its 16-byte block
view; its copy loops are register-sensitive.

See [memory-card-runtime.md](memory-card-runtime.md) for the dialog state
machines and [fm-online.md](fm-online.md) for the parallel two-player
addresses.
