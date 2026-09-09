#include "../types.h"
#include "display_object_stream_state.h"

s32 func_8004141C(DisplayObjectStreamState *object)
{
    object->field_5A = 0;
    return -1;
}

s32 func_80041428(DisplayObjectStreamState *object)
{
    object->field_58 = 0;
    return 1;
}

s32 func_80041434(void)
{
    return 1;
}

s32 func_8004143C(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 func_80041464(DisplayObjectStreamState *object, const u8 *data)
{
    object->flags ^= 0x800000;
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}
