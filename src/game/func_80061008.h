#ifndef MEMORIES_DECOMP_FUNC_80061008_H
#define MEMORIES_DECOMP_FUNC_80061008_H

#include "../types.h"

/* Builds the two card-list row sets at D_801A8000, one per player, anchored
 * at (x0, y0) and (x1, y1). Each gets a fresh display object whose render
 * hook is func_80060B38, and the viewport origin is reset to zero. The Trade
 * screen calls it once on entry. */
void func_80061008(s32 x0, s32 y0, s32 x1, s32 y1);

#endif
