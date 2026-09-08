#ifndef MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H
#define MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H

#include "../../types.h"

void MainMenu_RefreshTradeInventory(s32 slot, s32 force);
void MainMenu_DrawTradeOffersAndHighlights(void);
void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value);
void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID);
void func_80184030(s32 slot, s32 amount);
void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount);

#endif
