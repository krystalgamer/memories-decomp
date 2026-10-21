#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_LIST_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_LIST_H

#include "../types.h"

/* The gDisplayObject_ListRenderers entry for the list rooted at gDisplayObject_wSpriteListHead.
 * Runs each object's callback, then for every renderable object fills the
 * sprite primitive in the scratchpad at 0x1F800320, offsets it by the viewport
 * origin unless the object's bit 3 is set, and submits it through
 * DisplayObject_SubmitPacket. With bit 2 the position goes through
 * DisplayObject_ProjectRotationFacing's clip test first, which may ask through
 * gDisplayObject_fRenderRetry for the object to be run again,
 * and the packet built is the nine-word semi-transparent quad at 0x1F800344
 * instead. */
void DisplayObject_RenderSpriteList(void);

#endif
