#ifndef MEMORIES_DECOMP_FUNC_8001D240_H
#define MEMORIES_DECOMP_FUNC_8001D240_H

#include "../types.h"
#include "display_object.h"

/* Display-object update callback: the field-action state installs it at the
   object's 0x24 and also calls it directly (func_8001D240.c:7). */
void func_8001D240(DisplayObject *o);

#endif
