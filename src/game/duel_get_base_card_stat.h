#ifndef MEMORIES_DECOMP_DUEL_GET_BASE_CARD_STAT_H
#define MEMORIES_DECOMP_DUEL_GET_BASE_CARD_STAT_H

#include "../types.h"

/* Base attack or defence for a card id before any modifier is applied.
 * arg1 selects the stat: 0 is attack, 1 is defence. */
s32 Duel_GetBaseCardStat(s32 arg0, s32 arg1);

#endif
