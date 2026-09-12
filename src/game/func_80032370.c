#include "../types.h"
#include "duel_reward_setup.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "../unmatched.h"

void func_80032370(void)
{
    s16 *source = gDuel_awRecentCardDrops;
    u8 *base = (u8 *)source - 0x56C;
    s32 i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    s16 *current = source + DUEL_RECENT_CARD_DROP_COUNT - 1;
    s16 *destination;

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
