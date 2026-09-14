#ifndef MEMORIES_DECOMP_FRONTEND_SCENE_STATES_H
#define MEMORIES_DECOMP_FRONTEND_SCENE_STATES_H

#include "../types.h"

/* The frontend's scene states, the steps of the gDebugMenu_apfnPrimaryPageSteps table in
 * frontend_step_tables.c. Most latch bit 0x80 of D_8009B2EB on their first
 * call to do their setup, poll on later calls, and clear the byte when done.
 * DebugMenu_EnterDuel and DebugMenu_EnterDeckEditor act once without the latch. The number after
 * each name is its gDebugMenu_apfnPrimaryPageSteps index.
 *
 * func_80030C10 is in no table and has no caller in C. */
void func_80030C10(void);
void DebugMenu_UpdateTitleEntry(void); /* 10 */
void DebugMenu_UpdateMovieEntry(void); /* 13 */
void DebugMenu_UpdateLoadEntry(void); /* 14: runs the duel effect state with gDuel_bEffectState = 3 */
void DebugMenu_UpdateSaveEntry(void); /* 15: the same with gDuel_bEffectState = 4 */
void DebugMenu_UpdateTradeEntry(void); /* 16: polls func_8003FCD8, then selects mode 14 */
void DebugMenu_EnterDuel(void); /* 3: arms a duel through func_80024DC8 */
void DebugMenu_EnterDeckEditor(void); /* 8: calls func_80033C90 */

#endif
