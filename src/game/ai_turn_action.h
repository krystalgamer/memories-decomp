#ifndef MEMORIES_DECOMP_AI_TURN_ACTION_H
#define MEMORIES_DECOMP_AI_TURN_ACTION_H

#include "../types.h"
#include "duel_card.h"

int Ai_SelectMagicPlay(void);
s32 Ai_SelectFusionPlay(void);
s32 Ai_SelectHandPlay(void);

/* First positive comparison in grid order, not a maximum search. If no
 * occupied card exists, return the row's middle grid entry; otherwise -1. */
int Ai_FindFavorableFieldTarget(DuelCardRecord *source);
s32 Ai_FindFavorableFaceUpFieldTarget(DuelCardRecord *source);

s32 Ai_SelectFieldPlay(void);

#endif
