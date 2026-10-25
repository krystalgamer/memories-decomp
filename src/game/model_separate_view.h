#ifndef MEMORIES_DECOMP_MODEL_SEPARATE_VIEW_H
#define MEMORIES_DECOMP_MODEL_SEPARATE_VIEW_H

#include "../types.h"

/* Keeps the viewpoint out of the two model slots. For each slot with
 * is_active set (bar one whose tint[3] is at least 2 while the low 24
 * bits of tint are clear) it measures the horizontal distance from the
 * yaw-offset viewpoint (gModel_RView.vpx/vpz) to that slot and, when the
 * distance is below the slot's larger half-extent, pushes the viewpoint away
 * by the overlap; the slots themselves do not move. `mode` 0 only measures,
 * `min_extent` bounds the half-extents and the viewpoint offset, and `depth`
 * counts the recursive retries (up to three). Returns the limit of the last
 * move, or 0. */
s32 Model_SeparateViewFromSlots(s32 mode, s32 min_extent, s32 depth);

#endif
