#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

extern int gDuel_adwCardStats[];
extern int Duel_CalcGuardianStarMatchup(int, int);

s32 Duel_CalcGuardianStarBonus(DuelCardRecord *left, DuelCardRecord *right)
{
    int left_value, right_value;
    if (right == 0) return 0;
    if (left->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    if (right->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    return Duel_CalcGuardianStarMatchup(left_value, right_value);
}
