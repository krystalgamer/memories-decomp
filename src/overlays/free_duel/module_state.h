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

/* The linker symbol names the live low byte; module_state.c owns its complete
 * four-byte initialized storage so the following raw tail keeps its offset. */
extern u8 gFreeDuel_bScreenFlags;

#endif
