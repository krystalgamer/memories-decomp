#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_H

#include "../types.h"
#include "display_object.h"

/* Retail callers supply only object, while the cooldown path consumes the
 * ambient values left in $a1 and $a2. */
#ifdef DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS
void DisplayObject_UpdateCommandStream(DisplayObject *object);
#else
void DisplayObject_UpdateCommandStream(
    DisplayObject *object,
    s32 script,
    s32 attribute
);
#endif

#endif
