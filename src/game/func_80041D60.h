#ifndef MEMORIES_DECOMP_FUNC_80041D60_H
#define MEMORIES_DECOMP_FUNC_80041D60_H

#include "../types.h"
#include "display_object.h"

/* Retail callers supply only object, while the cooldown path consumes the
 * ambient values left in $a1 and $a2. */
#ifdef FUNC_80041D60_AMBIENT_ARGS
void func_80041D60(DisplayObject *object);
#else
void func_80041D60(DisplayObject *object, s32 script, s32 attribute);
#endif

#endif
