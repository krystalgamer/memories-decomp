#include "../types.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "mem_card_create_state.h"

void MemCardDialog_StepSave(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 0;
    }
    MemCardDialog_UpdateSave();
}

void MemCardDialog_StepNone(void)
{
}
