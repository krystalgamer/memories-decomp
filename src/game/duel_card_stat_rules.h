#ifndef MEMORIES_DECOMP_DUEL_CARD_STAT_RULES_H
#define MEMORIES_DECOMP_DUEL_CARD_STAT_RULES_H

#include "../types.h"

s32 Duel_CalcGuardianStarMatchup(s32, s32);

/* Base attack or defence for a card id before any modifier is applied.
 * arg1 selects the stat: 0 is attack, 1 is defence. */
s32 Duel_GetBaseCardStat(s32 arg0, s32 arg1);

#endif
