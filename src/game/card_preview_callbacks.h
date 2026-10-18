#ifndef MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H
#define MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H

#include "../types.h"
#include "display_object.h"

/* Draws the paired preview's frame, image, stat rows and card-detail tiles. */
void CardPreview_DrawDetailPanel(DisplayObject *object, s32 arg1);
/* Uses the stored variant for nonnegative reference-triangle facing area,
 * otherwise variant 2, then redraws and requests the display-list pass again. */
void CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area);

#endif
