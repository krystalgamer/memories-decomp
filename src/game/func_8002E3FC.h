#ifndef MEMORIES_DECOMP_FUNC_8002E3FC_H
#define MEMORIES_DECOMP_FUNC_8002E3FC_H

#include "../types.h"

/* Creates and registers one screen-space display object, returning it.
 *
 * It allocates a type 2 object, configures it through func_800404CC with a
 * fixed geometry and depth, sets DISPLAY_OBJECT_FLAG_SCREEN_SPACE in the flag
 * halfword at +0x08, hands it to func_80042918, and returns the pointer.
 *
 * The return type is void * because that is what the definition writes. It
 * builds a DisplayObject and casts to that type for the registration call,
 * but the returned pointer is handed back untyped, so this header does not
 * claim more than the definition does.
 *
 * campaign_load_scene_package.c is the only consumer and its local extern
 * already agreed
 * with this exactly. */
void *func_8002E3FC(void);

#endif
