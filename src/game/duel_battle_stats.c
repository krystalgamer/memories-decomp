#include "../types.h"
#include "duel_card.h"
#include "card_constants.h"

s32 Duel_CalcBattleAttack(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (u16)Duel_CalcCardStats(card)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

s32 Duel_CalcBattleDefense(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (Duel_CalcCardStats(card) >> 16)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}
