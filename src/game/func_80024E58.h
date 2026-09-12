#ifndef MEMORIES_DECOMP_FUNC_80024E58_H
#define MEMORIES_DECOMP_FUNC_80024E58_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: the terrain effect step. It reads
 * gDuel_bTerrain back after storing it and decrements in the same
 * expression. */
void DuelEffect_ApplyTerrain(void);

#endif
