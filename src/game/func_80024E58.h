#ifndef MEMORIES_DECOMP_FUNC_80024E58_H
#define MEMORIES_DECOMP_FUNC_80024E58_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: the terrain effect step. It reads
 * gDuel_bTerrain back after storing it and decrements in the same expression,
 * which is why the unit is built with its own flag set -- see the note in the
 * source before changing the spelling. */
void func_80024E58(void);

#endif
