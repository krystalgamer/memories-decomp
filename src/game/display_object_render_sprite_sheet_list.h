#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_SHEET_LIST_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_SHEET_LIST_H

#include "../types.h"

/* The gDisplayObject_ListRenderers entry for the single-headed list at
 * D_800EFE3C -- the one list whose head is a scalar rather than an entry of
 * D_800EFE38. Runs each object's callback, updates its command stream, and
 * renders the eligible sprite sheets. */
void DisplayObject_RenderSpriteSheetList(void);

#endif
