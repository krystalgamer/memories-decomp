# Trade readiness and final decision

Trade freezes a side's inventory interaction once that player marks the offer
ready. Each ready side receives a resident-created indicator. When both sides
are ready, the updater removes both indicators and opens a three-action menu:
commit/save, resume editing, or exit. This bundle names that complete lifecycle,
not the generic text system or the accepted three-position focus selector.

| NA entry/address | Identity | Contract |
| --- | --- | --- |
| `0x800610E0` | `MainMenu_CreateTradeReadyIndicator` | Channel 3/template 10, side-specific text branch and sprite at side times 160 |
| `0x800611D0` | `MainMenu_CreateTradeDecisionMenu` | Channel 3/template `0x20`, selected one of three color slots and attached cursor |
| `0x80185CC8` | `gTrade_abReady` | Two side-ready bytes; scalar view remains selected by `MAIN_MENU_TRADE_READY_AS_SCALAR` |
| `0x80185CC9` | `gTrade_bSide1Ready` | Separate symbol for the second ready byte, not another allocation |
| `0x80185CCF` | `gTrade_bDecisionMenuActive` | Gates final menu navigation and decisions |
| `0x80185CD0` | `gTrade_bCommitRequested` | Requests applying offers and starting the save |
| `0x80185CD1` | `gTrade_bWritePending` | Gates asynchronous write-result handling |

## Evidence

The updater calls the ready constructor with side 0 or 1 before setting its
ready byte. Cancelling clears the cancelling side; cancelling the final menu
recreates the other ready indicator. Both ready bytes trigger the decision
constructor. Selection 0 snapshots both save blocks, applies card-quantity
changes and calls `SaveData_RequestTradeWrite`; selection 1 clears both ready
bytes and resumes; selection 2 returns the exit decision.

The second ready byte is also read and written by its own symbol. The old
header's assertion that it was never cleared overlooked initialization,
successful-write and resume loops through the two-byte array. Preserve that
interior symbol and relocation identity, all byte declarations, the first-byte
scalar arm, and volatile casts at existing use sites.

These are views inside existing Main Menu state. The NA bindings remain based
on `gMainMenu_TradeState + 0x16EC/0x16ED/0x16F3/0x16F4/0x16F5`; no separate C
storage is introduced. Every European main-menu symbol file binds the names
directly at `0x80185EF4/EF5/EFB/EFC/EFD`. Japan, whose main-menu state layout
matches North America (`gTrade_abCursorRow` and `gTrade_bFocusPane` are already
bound at `0x80185CCA`/`0x80185CCE`), binds them at the NA addresses instead of
relying on Splat's auto-generated `D_` providers; the Japanese overlay match
verifies those addresses.

## Compatibility and matching

Both resident constructors preserve their signed 32-bit integer ABI and
unchecked index arithmetic. The ready constructor remains G0, the decision
constructor G0 split, under all seven authoritative manifests. The `s32`/`u8`
spellings are exact aliases of their original `int`/`unsigned char` types.
Box dimensions, callback indices, sprite tiles, ordering tables, depth offsets,
color writes and attached pointers are unchanged.

All overlay C bodies retain their original bytes. Canonical declarations
live in `trade_confirmation_widgets.h`, which the Trade updater includes
directly; the numeric `func_800610E0.h`/`func_800611D0.h` headers and the
`func_800610E0`/`func_800611D0` linker aliases are gone. Each Main Menu
import map binds the two constructors at their resident addresses and the
five state bytes under their names (US as `gMainMenu_TradeState` offsets,
Japan at the same absolute addresses, Europe at `+0x22C`), so the Trade
header no longer bridges `D_80185CC8`-`D_80185CD1` spellings. Existing
storage providers remain intact.

The accepted focus-selector name, offers/working-card tables, cursor and sort
state, generic text branch selector and color slots are not renamed. No
transaction arithmetic, quantity cap, save order, cancellation, error/result
handling or UI behavior changes. Historical attempts/fingerprints and compiler
profiles remain untouched; supporting probes compile actual translation units,
not synthetic or source-string test machinery.
