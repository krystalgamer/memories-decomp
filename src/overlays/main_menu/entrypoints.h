#ifndef MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H
#define MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H

#include "../../types.h"

/* Resident callers must load the main-menu image before using these entries. */
void MainMenu_InitTradeScreen(void);
s32 MainMenu_UpdateTradeScreen(void);

#endif
