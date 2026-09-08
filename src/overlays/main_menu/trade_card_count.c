#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"
#include "trade_helpers.h"

extern CardCountEntry D_801845FC[];

void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount)
{
    CardCountEntry *p;
    CardCountEntry *entry;
    u32 total;
    s32 offset;
    s32 i;

    i = 0;
    offset = slot * 2888;
    p = D_801845FC;
    while (i < CARD_COUNT) {
        entry = (CardCountEntry *)(offset + (s32)p);
        if (entry->id == id) {
            total = entry->count + amount;
            if (total < 0xFB) {
                entry->count = total;
            }
            return;
        }
        p++;
        i++;
    }
}
