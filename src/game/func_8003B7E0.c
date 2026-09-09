#include "../types.h"
#include "text_stream_commands.h"

u8 func_8003B7E0(TextStreamOwner *object)
{
    return *object->streams[object->stream_index]++;
}
