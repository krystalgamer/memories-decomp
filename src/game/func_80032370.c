#include "../types.h"
#include "card_constants.h"
#include "duel_reward_setup.h"

void func_80032370(void)
{
    s16 *source;
    u8 *base;
    s32 i;
    u32 current;
    s16 *destination;

    source = gDuel_awRecentCardDrops;
    base = (u8 *)((u32)source - DUEL_RECENT_DROPS_CHEST_DISTANCE);
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = (u32)(source + DUEL_RECENT_CARD_DROP_COUNT - 1);
    for (; i >= 0; i--, current -= sizeof(*source)) {
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
