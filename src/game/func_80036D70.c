#include "../types.h"
#include "func_80036D70.h"

u32 func_80036D70(TextStreamOwner *object)
{
    u8 **stream = &object->streams[object->stream_index];
    u8 *current = *stream;

    *stream = current + 4;
    return (current[3] << 24) | (current[2] << 16) |
           (current[1] << 8) | current[0];
}
