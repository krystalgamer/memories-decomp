#ifndef MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H
#define MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H

#include "../../types.h"

/* Resident callers must load the main-menu image before using these entries. */
void MainMenu_InitFrontendMenu(s32 unused, s32 menu);
s32 MainMenu_UpdateFrontendMenu(void);
void MainMenu_DestroyFrontendMenu(void);
void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle);
s32 MainMenu_UpdateValueSetup(void);
void MainMenu_FinishValueSetup(void);
void MainMenu_InitTradeScreen(void);
s32 MainMenu_UpdateTradeScreen(void);
void MainMenu_ReleaseTradeDisplayHandles(void);

#endif
