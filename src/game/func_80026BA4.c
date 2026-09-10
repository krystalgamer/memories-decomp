#include "../types.h"
#include "card_constants.h"
#include "duel_action_lock.h"
#include "func_80026BA4.h"

void func_80026BA4(int value, int flag)
{
    int index;
    if (((unsigned)(value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID) < DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        ((unsigned)(value - DUEL_EFFECT_SECOND_BLOCK_CARD_ID) < DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
        if (value < DUEL_EFFECT_SECOND_BLOCK_CARD_ID)
            index = value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID;
        else {
            index = value - (DUEL_EFFECT_SECOND_BLOCK_CARD_ID - DUEL_EFFECT_CARD_BLOCK_SIZE);
            if (value == DUEL_DARK_MAGIC_RITUAL_CARD_ID)
                index = DUEL_DARK_MAGIC_RITUAL_EFFECT_INDEX;
        }
        D_8009B1A8 = index;
        D_8009B1D2 = value;
        D_8009B220 = DUEL_CARD_EFFECT_FLAG_ACTIVE;
        if (flag)
            D_8009B220 = DUEL_CARD_EFFECT_FLAG_ACTIVE | DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER;
    }
}
