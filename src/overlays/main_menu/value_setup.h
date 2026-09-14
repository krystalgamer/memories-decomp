#ifndef MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H
#define MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H

#include "../../types.h"

void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle);
s32 MainMenu_UpdateValueSetup(void);
void MainMenu_FinishValueSetup(void);
void MainMenu_StartValueWidgetTween(s32 index, s32 value);
void MainMenu_DrawValueSetup(void);
s32 MainMenu_CountDecimalDigits(s32 value);

#endif
