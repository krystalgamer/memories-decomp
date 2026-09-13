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

extern u32 gFreeDuel_dwScreenFlagsStorage asm("gFreeDuel_bScreenFlags");
#define gFreeDuel_bScreenFlags (*(u8 *)&gFreeDuel_dwScreenFlagsStorage)

#endif
