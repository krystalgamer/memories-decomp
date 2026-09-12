#include "../types.h"
#include "duel_reward_setup.h"
#include "card_constants.h"

void func_80032370(void)
{
    s16 *source = gDuel_awRecentCardDrops;
    s16 *destination;
    s32 i;
    /* The chest byte table precedes the recent-drop list by 0x56C bytes. */
    u8 *base = (u8 *)((u32)source - 0x56C);
    u32 current;

    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = (u32)(source + DUEL_RECENT_CARD_DROP_COUNT - 1);
    for (; i >= 0; i--, current -= sizeof(s16)) {
        if (*(s16 *)current != 0 && base[*(s16 *)current - 1] == 0)
            *(s16 *)current = 0;
    }
    destination = source;
    for (i = 0; i < DUEL_RECENT_CARD_DROP_COUNT; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}
