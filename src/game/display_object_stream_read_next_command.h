#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_READ_NEXT_COMMAND_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_READ_NEXT_COMMAND_H

#include "../types.h"
#include "../ygo_types.h"
#include "display_object.h"

/* Steps the object's command stream. Only object is read; its caller,
 * DisplayObject_UpdateCommandStream, also passes the script and attribute
 * it resolved and the object again. */
void DisplayObjectStream_ReadNextCommand(
    DisplayObjectStreamState *object, s32 script, s32 attribute, DisplayObject *owner
);

#endif
