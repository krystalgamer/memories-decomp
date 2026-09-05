#include "../types.h"
#include "card_constants.h"

typedef struct { char pad_00[0xC]; s16 id; char pad_0E[8]; u16 flags; } Entry;
extern int gDuel_adwCardStats[];
extern int Duel_CalcGuardianStarMatchup(int, int);

int Duel_CalcGuardianStarBonus(Entry *left, Entry *right)
{
    int left_value, right_value;
    if (right == 0) return 0;
    if (left->flags & 0x200)
        left_value =
            (gDuel_adwCardStats[left->id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        left_value =
            (gDuel_adwCardStats[left->id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    if (right->flags & 0x200)
        right_value =
            (gDuel_adwCardStats[right->id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        right_value =
            (gDuel_adwCardStats[right->id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    return Duel_CalcGuardianStarMatchup(left_value, right_value);
}
