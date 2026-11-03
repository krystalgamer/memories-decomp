#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_HAS_RESOURCE_ENTRY_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_HAS_RESOURCE_ENTRY_H

#include "../types.h"

/* Walks three levels of halfword offset tables rooted at `b`: entry `x` of the
 * root gives the offset of a second table, entry `y` of that gives a third,
 * and entry `z` of the third must be non-zero. Returns 1 when all three are
 * non-zero, 0 at the first zero. DisplayEffect_BuildResourceObjects asks it
 * about the table at gDisplay_abResourceBank0 or gDisplay_abResourceBank1. */
int DisplayEffect_HasResourceEntry(
    unsigned char *base, int group, int row, int entry);

#endif
