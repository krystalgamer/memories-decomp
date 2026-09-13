#include "../types.h"
#include "duel_reward_setup.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "../unmatched.h"

void func_80032370(void)
{
    s16 *source;
    u8 *base;
    s32 i;
    s16 *current;
    s16 *destination;

    source = gDuel_awRecentCardDrops;
    base = (u8 *)source - 0x56C;
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = source + DUEL_RECENT_CARD_DROP_COUNT - 1;
    for (; i >= 0; i--, current--) {
        if (*current != 0 && base[*current - 1] == 0)
            *current = 0;
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
