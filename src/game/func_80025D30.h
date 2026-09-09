#ifndef MEMORIES_DECOMP_FUNC_80025D30_H
#define MEMORIES_DECOMP_FUNC_80025D30_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: the field-wide stat-penalty sweep, one
 * slot per 16 frames across the acting side's second grid row. Each step spawns
 * a type-0xD object at the card's model position and drops one stat level for
 * Spellbinding Circle, two otherwise; after the fifth slot it sets bit 0x40 of
 * D_8009B220 and waits there until bit 0 of D_8009B260 clears. */
void func_80025D30(void);

#endif
