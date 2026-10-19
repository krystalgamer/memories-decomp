#ifndef MEMORIES_DECOMP_DUEL_EFFECT_APPLY_TERRAIN_H
#define MEMORIES_DECOMP_DUEL_EFFECT_APPLY_TERRAIN_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: the terrain effect step. It reads
 * gDuel_bTerrain back after storing it and decrements in the same
 * expression. */
void DuelEffect_ApplyTerrain(void);

#endif
