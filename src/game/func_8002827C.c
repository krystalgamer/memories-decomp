#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_card.h"

s32 func_8002827C(DuelCardRecord *object)
{
    if (object->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
        s32 *table = gDuel_adwCardStats;
        s32 index = object->card_id - 1;

        return (table[index] >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
               CARD_STAT_GUARDIAN_STAR_MASK;
    } else {
        s32 *table = gDuel_adwCardStats;
        s32 index = object->card_id - 1;

        return (table[index] >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
               CARD_STAT_GUARDIAN_STAR_MASK;
    }
}
