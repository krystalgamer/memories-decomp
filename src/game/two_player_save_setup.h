#ifndef MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H
#define MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H

#include "../types.h"

/* The message the two-save load shows when it rejects the pair:
 * mem_card_dialog.c sets it to 0x29, 40 and 36 at different failures
 * and reads it
 * back. All three spell it plain u8. */
extern u8 gSaveData_PairMessageCode;

/* gSaveData_LoadPairState and gSaveData_PairSetupState are one byte each (c_symbols.ld:278 and :280);
 * no unit defines either. Six units reach them: four as a plain u8, two as
 * an unsized u8 array read only at [0], which is the same -G8 lever
 * gMemCardDialog_CardNumberArg keeps in mem_card.h.
 *
 * gSaveData_PairSetupState: SaveData_UpdateTradeLoad and SaveData_UpdateDuelLoad in
 * mem_card_dialog.c test bit 0x80
 * clear, set it and store
 * gSaveData_PairMessageCode; DebugMenu_UpdateTradeEntry (frontend_scene_states.c),
 * DebugMenu_UpdateTwoPlayerDuelEntry (debug_menu_two_player_entry.c:19) and
 * MainMenu_UpdateFrontendMenu (cases 3 and 2 of
 * its gMain_bMenuID switch, src/overlays/main_menu/frontend.c) store 0.
 *
 * gSaveData_LoadPairState: SaveData_UpdateLoadPair in mem_card_dialog.c masks it
 * with 0xF, tests bits 0x80 and 0x40, stores 1, 0x82, 2, 3, 0xA and 0xB,
 * ORs 0x80, 0x40 and 0xC0 and ANDs 0xBF into it; SaveData_UpdateDuelLoad
 * stores 10 in the same unit; the three functions above store 0.
 *
 * u8 because the grouped unit's former sources already declared it u8 and
 * matched, and the retail loads are lbu. Retail addressing:
 * DebugMenu_UpdateTradeEntry and DebugMenu_UpdateTwoPlayerDuelEntry store
 * both through lui $at (debug_menu_primary_entries.s:11-14,
 * debug_menu_two_player_entry.s:11-14), which is the .data arm; the other
 * four units are
 * gp-relative or, in the main_menu overlay, built at -G0, which is the
 * plain arm. Initial value not read. notes/fm-online.md:177-187 records the
 * gSaveData_LoadPairState store at 0x8003FAE8 and leaves its meaning unresolved. */
#ifdef D_8009B3EA_IN_DATA
extern u8 gSaveData_LoadPairState __attribute__((section(".data")));
#else
extern u8 gSaveData_LoadPairState;
#endif
#ifdef D_8009B3ED_IN_DATA
extern u8 gSaveData_PairSetupState __attribute__((section(".data")));
#else
extern u8 gSaveData_PairSetupState;
#endif

/* The setup/write API at the end of mem_card_dialog.c. Both functions
 * were reached through local externs
 * before this header existed, and every one of those declarations already
 * agreed with its definition.
 *
 * SaveData_UpdateDuelLoad reports a status its callers poll:
 * MainMenu_UpdateFrontendMenu (src/overlays/main_menu/frontend.c) stores the result for
 * main-menu item 2 and debug_menu_two_player_entry.c tests it
 * before entering the two-player duel setup. On top of the trade load it
 * requires a full deck in both saves and converts both player names.
 *
 * SaveData_RequestTradeWrite stamps integrity over both slots of the pair at
 * tent_TradeSaveBuffer, records the second slot in
 * gMemCard_pSecondaryTransferCursor, and starts dialog step 4
 * to write them back. MainMenu_UpdateTradeScreen
 * (src/overlays/main_menu/trade_update.c) calls it once, after moving the
 * traded card counts. */
s32 SaveData_UpdateDuelLoad(void);
void SaveData_RequestTradeWrite(void);

#endif
