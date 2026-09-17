#ifndef MEMORIES_DECOMP_FUNC_8002ABB4_H
#define MEMORIES_DECOMP_FUNC_8002ABB4_H

#include "../types.h"
#include "display_object.h"

/* Spawns a fading afterimage of `src`: a fresh general display object that
 * copies its position, scale and colour words, sits `add` depth steps from
 * it, and runs func_8002A9C0 as its per-frame callback. Returns the copy. */
DisplayObject *func_8002ABB4(DisplayObject *src, int add);

#endif
