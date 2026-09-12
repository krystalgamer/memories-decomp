#ifndef MEMORIES_DECOMP_DUEL_SWORDS_EFFECT_H
#define MEMORIES_DECOMP_DUEL_SWORDS_EFFECT_H

#include "../types.h"

#define DUEL_SWORDS_DURATION_TURNS 3
#define DUEL_SWORDS_INITIAL_COUNTER (DUEL_SWORDS_DURATION_TURNS + 1)

void DuelEffect_StartSwords(void);
void DuelEffect_ApplySwords(void);

#endif
