#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_PROPERTY_TRANSITIONS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_PROPERTY_TRANSITIONS_H

#include "../types.h"
#include "display_object.h"

/* The unit's two display-object update callbacks. Neither is called: the
   field-action state stores each one's address into an object's 0x24 field
   (src/candidates/func_8001D670.c:524, :539, :600, :621), so the declarations
   have to keep the shape that field expects. */
void func_8001D344(DisplayObject *object);
void func_8001D3C4(DisplayObject *o);

#endif
