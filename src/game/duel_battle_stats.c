#include "../types.h"
#include "card_constants.h"

extern int Duel_CalcCardStats(void *);
extern int Duel_CalcGuardianStarBonus(void *, int);

int Duel_CalcBattleAttack(void *arg0, int arg1)
{
    int value = (unsigned short)Duel_CalcCardStats(arg0)
        + Duel_CalcGuardianStarBonus(arg0, arg1);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

int Duel_CalcBattleDefense(void *arg0, int arg1)
{
    int value = (Duel_CalcCardStats(arg0) >> 16)
        + Duel_CalcGuardianStarBonus(arg0, arg1);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}
