# Functions no traced route has called

Several game-owned functions have no caller the static tools can find: no C
call, no `jal`, and no stored pointer in the executable or any extracted
DATA file. Under #418, a family of watch traces put an execution breakpoint
on each of them, checked the entry instruction word first, and used
`Main_RunFrameServices` as a control.

Every run below used the interpreter CPU, reported no signature mismatch,
and had control hits on almost every frame. Silence in these runs is
therefore real negative evidence.

| Route | Frames | Control hits |
| --- | ---: | ---: |
| Main menu, Library card list and a monster's 3-D view, Build Deck; watched only the two grid helpers and `Model_GetSlotScale` (`unreferenced_helper_callers`, #7739) | 2,601 | 2,448 |
| Title screen left idle: the intro movie replayed and returned to "Push Start Button" (`title_idle_watch`) | 5,400 | 4,466 |
| Library 3-D view of Rare Fish (230), plus Horn of Light (313) and Yami (335), which have no 3-D model (`library_3d_watch`) | 7,374 | 6,682 |
| Free Duel against Villager2, holding L2 to move the field cursor over every zone of the board before playing a card (`duel_field_watch`) | 7,622 | 7,618 |

None of the last three routes called any of these functions or wrote
`D_8009AFA6`:

| Function | Static role | Notes |
| --- | --- | --- |
| `CardGrid_GetColumnX`, `CardGrid_GetRowY` | Library grid column/row coordinates | [`card-grid-coordinate-helpers.md`](card-grid-coordinate-helpers.md). The duel board's cursor does not use them. |
| `Model_GetSlotScale` | Model-slot scale getter | [`model-slot-properties.md`](model-slot-properties.md) |
| `ViewState_ApplyAlternateOrbit` | Alternate orbit viewpoint | [`view-state-current.md`](view-state-current.md) |
| `Library_CheckCardInChest` | Chest-count test | Library 3-D and duel board routes |
| `SD_CdBgmPlay` | BGM command `0xA000` wrapper | [`bgm-frontend-commands.md`](bgm-frontend-commands.md). The intro movie's replay did not use it. |
| `Model_GetSlot0OffsetWorldPosition` | Slot-0 world-position helper | [`model-view-coordinates.md`](model-view-coordinates.md) |
| `func_8005C374` | Stores three bytes `D_8009B4A0..A2` | Not called during the intro-movie replay either. No matching C reads those bytes. |
| `func_80058DC0` | Returns `D_8009AFA6` | [`graphics-frame-views.md`](graphics-frame-views.md). The byte was never written in any route. |
| `Model_MapToWireframePrimitiveHandler`, `Model_MapToBasePrimitiveHandler` | Handler-key maps | [`model-structures.md`](model-structures.md). Also silent in the Library 3-D view. |

Together with the earlier 3-D battle traces in
[`model-structures.md`](model-structures.md), these routes cover the title
loop, menus, Library, Build Deck, the duel board and 3-D battles. Each one
ruled out a screen; none found a caller. The routes still not covered are
the campaign map, dialogue, Trade, Password, the duel result screens, fusion
and ritual summons, and the credits. Names that are still address-based stay
that way: never being called is not evidence of a role, and these results do
not show that the code is unreachable.

A narrower menu tour (`screen_tour_watch`, #7930; 9,572 frames, 9,086
control hits, 40 screen changes) watched only `func_80058DC0` and stores to
bytes one and two of `gGraphics_FrameStepControl`. The route went through
the main menu, Password, Library (including a card's 3-D view), Trade with
its memory-card dialogue, campaign dialogue with Simon Muran, the world map,
the Card Shop with a save, Return to Title, and the Free Duel menu. There
were no getter calls and no writes to either byte. The watch caught 9,086
other stores to the control word, about one per frame, and none of them
touched bytes one or two. Password, Trade, the campaign map and
dialogue are now covered for this getter only. Duel results, fusion and
ritual summons, and the credits remain uncovered for all of them.

## Resident globals with no meaningful reader or writer

`resident_dead_globals_watch` (#418; 18,000 vsyncs, 16,735 control hits,
interpreter CPU, all 31 signature words verified) put read and write
breakpoints on resident globals that the static scan leaves without a
meaningful reader or writer, plus execution breakpoints on
`func_8005C374` and `func_8003D32C`. The route was a soft reset, a loaded
save, the campaign (Card Shop, the palace duel against Simon Muran, back to
the Card Shop and Return to Title), then the debug menu (entered by
patching the main-menu selection byte at `0x80184594` to `0x0B`) with its
BustUp and MOVIE entries. MOVIE hung input with rendering still running and
played no movie.

Every access outside the static sites was a boot-time clear: the BIOS
before the executable loads, and the resident `entrypoint` (`0x800129D8`)
zeroing `.bss`. Apart from those:

| Global | Static sites | What the run saw |
| --- | --- | --- |
| `D_8009B0A4`, `D_8009B0D4` | read by `Main_RunFrameServices` | read 17,009 times each, always 0; never written |
| `D_8009B0B0`, `D_8009B0BC` | `Main_RunFrameServices` copies the two above into them | written 274 times each, always 0 |
| `D_8009B0D1` | `Main_Init`, `Main_ApplyMenuSelection`, `SaveData_RequestLoad` store 0 | stored 0 twice; never read |
| `D_8009B268` | stored beside menu requests; `Main_RunMenu` passes it to `MainMenu_InitFrontendMenu` | set to 1 by `Main_ApplyMenuSelection` (campaign start, debug menu) and `Script_OpSavePrompt`, read once by `Main_RunMenu` |
| `D_8009B2B2` | `DebugMenu_Init` stores `0x80`, `DebugMenu_EnterMappedMode` stores 0 | set to `0x80` on entering the debug menu; never read |
| `D_8009B2DE` | `DebugMenu_UpdateBustUpEntry` zeroes, then tests it | zeroed and read back as 0 by the BustUp entry |
| `D_8009B4A0..A2` | written only by `func_8005C374` | never accessed |

Neither `func_8005C374` nor `func_8003D32C` was called, including when the
debug menu's MOVIE entry was chosen. The runtime agrees with the static
scan on this route: the frame-service pair stays zero, and the
write-only bytes are never read. The names stay address-based, because
being dead on one route is not a role.
