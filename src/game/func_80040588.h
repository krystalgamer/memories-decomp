#ifndef MEMORIES_DECOMP_FUNC_80040588_H
#define MEMORIES_DECOMP_FUNC_80040588_H

#include "../types.h"

/* The D_80090FB0 renderer for the list rooted at D_800EFE3A. Runs each
 * object's callback, then for every renderable object fills the sprite
 * primitive in the scratchpad at 0x1F800320, offsets it by the viewport origin
 * unless the object's bit 3 is set, and submits it through func_80042188. With
 * bit 2 the position goes through func_80041F90's clip test first, which may
 * ask through D_8009B424 for the object to be run again, and the packet built
 * is the nine-word semi-transparent quad at 0x1F800344 instead. */
void func_80040588(void);

#endif
