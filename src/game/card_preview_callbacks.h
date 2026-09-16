#ifndef MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H
#define MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H

#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"

void func_80028B08(DisplayObject *object, s32 arg1);
/* Applies the preview object's stored card variant, or fallback variant 2,
 * then redraws and requests the current display-list pass to rerun. */
void CardPreview_UpdateVariant(DisplayObjectConfig *object, s32 arg1);

#endif
