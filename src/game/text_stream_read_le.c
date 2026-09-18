#include "../types.h"
#include "duel_effect.h"
#include "text_stream_read_u16_le.h"
#include "text_stream_read_u32_le.h"

int TextStream_ReadU16LE(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_58];
    u8 *current = *stream;
    *stream = current + 2;
    return current[0] | (current[1] << 8);
}

u32 TextStream_ReadU32LE(TextStreamOwner *object)
{
    u8 **stream = &object->streams[object->stream_index];
    u8 *current = *stream;

    *stream = current + 4;
    return (current[3] << 24) | (current[2] << 16) |
           (current[1] << 8) | current[0];
}
