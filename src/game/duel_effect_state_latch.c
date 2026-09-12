#include "../types.h"
#include "duel_effect_state_latch.h"

extern u8 gDuel_bEffectHandlerFlags;

int DuelEffect_MarkStateInitialized(void)
{
    unsigned char value = gDuel_bEffectHandlerFlags;

    if (!(value & 0x80)) {
        gDuel_bEffectHandlerFlags = value | 0x80;
        return 0;
    }
    return 1;
}
