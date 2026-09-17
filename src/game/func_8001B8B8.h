#ifndef MEMORIES_DECOMP_FUNC_8001B8B8_H
#define MEMORIES_DECOMP_FUNC_8001B8B8_H

#include "../types.h"
#include "duel_selection_layout.h"

/* Dims every hand card, then restores the one under the side's cursor
 * (side->field_0E) unless side->field_15 says the side is not picking from
 * its hand. */
void func_8001B8B8(DuelSelectionRecord *side);

#endif
