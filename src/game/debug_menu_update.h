#ifndef MEMORIES_DECOMP_DEBUG_MENU_UPDATE_H
#define MEMORIES_DECOMP_DEBUG_MENU_UPDATE_H

#include "../types.h"

void DebugMenu_Update(void);

/* Entry 6 of the frontend step table gDebugMenu_apfnPrimaryPageSteps (debug_menu_update.c).
 * On entry it loads campaign scene package 0. Each frame it then runs
 * DebugMenu_NoOpBustUpFrame, and while D_8009B2DE is non-zero it passes
 * gDisplayEffect_aMenuRecords and
 * Rand_GetInterval(4) to DisplayEffect_UpdateMenuRecord. Once D_8009B2DE is zero it makes two display objects
 * renderable and clears the step byte. */
void DebugMenu_UpdateBustUpEntry(void);

#endif
