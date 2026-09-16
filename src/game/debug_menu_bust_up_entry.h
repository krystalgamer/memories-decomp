#ifndef MEMORIES_DECOMP_DEBUG_MENU_BUST_UP_ENTRY_H
#define MEMORIES_DECOMP_DEBUG_MENU_BUST_UP_ENTRY_H

#include "../types.h"

/* Entry 6 of the frontend step table gDebugMenu_apfnPrimaryPageSteps (frontend_step_tables.c).
 * On entry it loads campaign scene package 0. Each frame it then runs
 * func_8003134C, and while D_8009B2DE is non-zero it passes D_800EB010 and
 * Rand_GetInterval(4) to DisplayEffect_UpdateMenuRecord. Once D_8009B2DE is zero it makes two display objects
 * renderable and clears the step byte. */
void DebugMenu_UpdateBustUpEntry(void);

#endif
