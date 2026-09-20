#ifndef MEMORIES_DECOMP_FUNC_8002E3FC_H
#define MEMORIES_DECOMP_FUNC_8002E3FC_H

#include "../types.h"
#include "display_object.h"

/* Creates and registers one screen-space display object, returning it.
 *
 * It allocates a type 2 object, configures it through DisplayObject_ConfigureSpriteAtPosition with a
 * fixed geometry and depth, sets DISPLAY_OBJECT_FLAG_SCREEN_SPACE in the flag
 * halfword at +0x08, hands it to DisplayObject_SelectOrderingTable1, and returns the pointer.
 *
 * Both callers store the result in the typed D_8009B2A0 DisplayObject owner. */
DisplayObject *func_8002E3FC(void);

#endif
