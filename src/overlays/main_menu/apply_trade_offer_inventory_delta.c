#include "../../types.h"
#include "trade_helpers.h"

extern u16 D_80185C9C[][11];

void MainMenu_ApplyTradeOfferInventoryDelta(s32 slot, s32 amount)
{
    s32 i;

    i = 0;
    while (i < D_80185C9C[slot][0]) {
        MainMenu_AdjustTradeCardCount(slot, D_80185C9C[slot][i + 1], (u32)amount);
        i++;
    }
}
