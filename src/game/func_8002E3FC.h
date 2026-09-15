#ifndef MEMORIES_DECOMP_FUNC_8002E3FC_H
#define MEMORIES_DECOMP_FUNC_8002E3FC_H

#include "../types.h"

/* Creates and registers one screen-space display object, returning it.
 *
 * It allocates a type 2 object, configures it through func_800404CC with a
 * fixed geometry and depth, sets DISPLAY_OBJECT_FLAG_SCREEN_SPACE in the flag
 * halfword at +0x08, hands it to func_80042918, and returns the pointer.
 *
 * The implementation uses DisplayObject for construction and registration.
 * The returned pointer is still handed to the untyped D_8009B2A0 owner, so
 * this public return remains void *.
 *
 * campaign_load_scene_package.c is the only consumer and its local extern
 * already agreed
 * with this exactly. */
void *func_8002E3FC(void);

#endif
