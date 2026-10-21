#ifndef MEMORIES_DECOMP_MODEL_SEPARATE_VIEW_H
#define MEMORIES_DECOMP_MODEL_SEPARATE_VIEW_H

#include "../types.h"

/* Separates the two model records when their projected distance is below
 * the larger paired half-extent, pushing the reference vector gModel_RView
 * apart by the overlap; `mode` 0 only measures, `min_extent` bounds the
 * half-extents, and `depth` counts the recursive retries (up to three).
 * Returns the extent that moved, or 0. */
s32 Model_SeparateViewFromSlots(s32 mode, s32 min_extent, s32 depth);

#endif
