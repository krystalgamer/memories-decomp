#ifndef MEMORIES_DECOMP_CARD_LIST_CREATE_DECK_BOX_STAT_ROWS_H
#define MEMORIES_DECOMP_CARD_LIST_CREATE_DECK_BOX_STAT_ROWS_H

#include "../types.h"

/* Builds the two card-list row sets at D_801A8000, one per player, anchored
 * at (x0, y0) and (x1, y1). Each gets a fresh display object whose render
 * hook is CardList_RenderDeckBoxStats, and the viewport origin is reset to
 * zero. The Trade
 * screen calls it once on entry. */
void CardList_CreateDeckBoxStatRows(s32 x0, s32 y0, s32 x1, s32 y1);

#endif
