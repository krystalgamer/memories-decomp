#ifndef MEMORIES_DECOMP_CARD_LIST_DECK_BOX_H
#define MEMORIES_DECOMP_CARD_LIST_DECK_BOX_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "card_list_rows.h"

/* Draws a deck box's six type-count digits and its seven card stat rows into
 * the ordering table. CardList_CreateDeckBoxStatRows does not call this; it stores the address
 * in a display object's field_4C, so the object's own draw pass supplies both
 * arguments. That is why the declaration it replaced could say (void) without
 * the build noticing. */
void CardList_RenderDeckBoxStats(DisplayObject *obj, GsOT *ot);

void CardList_BuildRowSetTextBoxes(u16 *entries, s32 side, s32 flag, s32 ignored);

/* Builds the two card-list row sets at gFile_StagingBuffer, one per player, anchored
 * at (x0, y0) and (x1, y1). Each gets a fresh display object whose render
 * hook is CardList_RenderDeckBoxStats, and the viewport origin is reset to
 * zero. The Trade
 * screen calls it once on entry. */
void CardList_CreateDeckBoxStatRows(s32 x0, s32 y0, s32 x1, s32 y1);

#endif
