#ifndef MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H
#define MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H

#include "../types.h"

/* The setup/write API at the end of two_player_save_runtime.c. Both functions
 * were reached through local externs
 * before this header existed, and every one of those declarations already
 * agreed with its definition.
 *
 * SaveData_UpdateDuelLoad reports a status its callers poll:
 * MainMenu_UpdateFrontendMenu (now a build-integrated candidate,
 * src/candidates/main_menu/func_80180390.c) stores the result for
 * main-menu item 2 and async_state_poll.c tests it
 * before entering the two-player duel setup. On top of the trade load it
 * requires a full deck in both saves and converts both player names.
 *
 * SaveData_RequestTradeWrite stamps integrity over both slots of the pair at
 * D_801D1880, records the second slot in D_8009B3E0, and starts dialog step 4
 * to write them back. MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c) calls it once, after moving the
 * traded card counts. */
s32 SaveData_UpdateDuelLoad(void);
void SaveData_RequestTradeWrite(void);

#endif
