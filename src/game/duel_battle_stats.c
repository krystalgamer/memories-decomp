#include "../types.h"
#include "card_constants.h"
#include "display_object.h"
#include "duel_battle_stats.h"
#include "duel_calc_guardian_star_matchup.h"
#include "duel_card.h"

/* Battle arithmetic between two duel cards, built up in address order.

   Duel_CalcGuardianStarBonus reads each card's active guardian star from
   gDuel_adwCardStats and asks the wheel for the signed bonus. The attack and
   defence calculators add that bonus to the card's own stat and clamp it.
   func_8001EFD4 then picks attack or defence by each card's position and
   compares the result against the opponent's stat, for the AI's field scans.

   These were four sources at two profiles, gcc_2_8_1_g8 for the calculators
   and gcc_2_8_1_g8_split for the bonus and the comparison. The calculators
   compile to identical objects at gcc_2_8_1_g8_split, so the unit builds
   there. It is bounded below by the display-object motion pair and above by
   the attack-trap selector, which is a different mechanism. */

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

s32 func_8001EFD4(DisplayObject *left, DisplayObject *right)
{
    DuelCardRecord *a = &D_801A7AD8[left->field_6A];
    DuelCardRecord *b;
    s32 packed;
    s32 wanted;
    s32 actual;

    if (right == 0)
        return Duel_CalcCardStats(a) & 65535;
    b = &D_801A7AD8[right->field_6A];
    packed = Duel_CalcCardStats(b);
    wanted = packed & 65535;
    if (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        wanted = (u32)packed >> 16;
    if (a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        actual = Duel_CalcBattleDefense(a, b);
    else
        actual = Duel_CalcBattleAttack(a, b);
    if (actual != wanted)
        return actual - wanted;
    if ((a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) ||
        (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION))
        return 0;
    return -1;
}
