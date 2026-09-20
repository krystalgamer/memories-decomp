#ifndef MEMORIES_DECOMP_FUNC_80051350_H
#define MEMORIES_DECOMP_FUNC_80051350_H

#include "../types.h"

/* Separates the two model records when their projected distance is below
 * the larger paired half-extent, pushing the reference vector D_800F56F0
 * apart by the overlap; `mode` 0 only measures, `min_extent` bounds the
 * half-extents, and `depth` counts the recursive retries (up to three).
 * Returns the extent that moved, or 0. */
s32 func_80051350(s32 mode, s32 min_extent, s32 depth);

#endif
