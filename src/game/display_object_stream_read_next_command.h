#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_READ_NEXT_COMMAND_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_READ_NEXT_COMMAND_H

#include "../types.h"
#include "../ygo_types.h"
#include "display_object.h"

/* The stream dispatcher consumes only ctx. func_80041D60 nevertheless emits
 * the retail four-argument call sequence: script and attribute are selected
 * on the first-time path or ambient on the cooldown path, and ctx is repeated
 * in $a3. */
#ifdef DISPLAY_OBJECT_STREAM_READ_NEXT_COMMAND_WIDE_CALL
void DisplayObjectStream_ReadNextCommand(
    DisplayObject *ctx, s32 script, s32 attribute, DisplayObject *ctx_again
);
#else
void DisplayObjectStream_ReadNextCommand(DisplayObjectStreamState *object);
#endif

#endif
