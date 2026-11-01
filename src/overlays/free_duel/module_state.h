#ifndef MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_MODULE_STATE_H
#define MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_MODULE_STATE_H

#include "../../types.h"
#include "../../game/display_object.h"

#define FREE_DUEL_GRID_STATE_CAPACITY 40
#define FREE_DUEL_SPARKLE_POOL_CAPACITY 16

/* One byte per grid cell, non-zero when the cell holds a selectable duelist. */
extern u8 gFreeDuel_abGridAvailable[FREE_DUEL_GRID_STATE_CAPACITY];

/* The scrollbar thumb and cursor use the same display-object allocation and
 * layout as the cursor-trail sparkle pool. */
extern DisplayObject *gFreeDuel_pThumbWidget;
extern DisplayObject *gFreeDuel_apSparklePool[
    FREE_DUEL_SPARKLE_POOL_CAPACITY
];
extern DisplayObject *gFreeDuel_pCursorWidget;

/* The consumers read the live low byte. module_state.c selects the word arm
 * to define the complete four-byte initialized storage, so the following raw
 * tail keeps its offset (a separate byte definition would be padded by GCC
 * rather than followed by the three retail padding bytes). */
#ifdef FREE_DUEL_SCREEN_FLAGS_WORD
extern u32 gFreeDuel_bScreenFlags;
#else
extern u8 gFreeDuel_bScreenFlags;
#endif

#endif
