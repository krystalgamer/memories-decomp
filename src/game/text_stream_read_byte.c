#include "../types.h"
#include "text_stream_commands.h"

u8 TextStream_ReadByte(TextStreamOwner *object)
{
    return *object->streams[object->stream_index]++;
}
