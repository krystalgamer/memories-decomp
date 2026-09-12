#include "../types.h"
#include "duel_effect_state_latch.h"

int DuelEffect_MarkStateInitialized(void)
{
    unsigned char value = gDuel_bEffectHandlerFlags;

    if (!(value & DUEL_EFFECT_DIALOG_FLAG_CREATED)) {
        gDuel_bEffectHandlerFlags = value | DUEL_EFFECT_DIALOG_FLAG_CREATED;
        return 0;
    }
    return 1;
}
