#include "../types.h"
#include "duel_action_lock.h"

s32 DuelEffect_MarkInitialized(void)
{
    u16 value = gDuel_wCardEffectFlags;

    if (!(value & 0x80)) {
        gDuel_wCardEffectFlags = value | 0x80;
        return 0;
    }
    return 1;
}

void DuelEffect_ClearCardEffect(void)
{
    gDuel_wCardEffectFlags = 0;
}
