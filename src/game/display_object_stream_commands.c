#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
#include "display_object_stream_state.h"

s32 DisplayObjectStream_JumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 DisplayObjectStream_ToggleFlagAndJumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->flags ^= 0x800000;
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 DisplayObjectStream_JumpToRandomOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    int i = rand() % data[0];
    unsigned hi, lo;
    u8 *base;

    data += i * 2 + 1;
    hi = data[1];
    lo = data[0];
    base = object->base;
    object->field_58 = 0;
    object->current = base + ((hi << 8) | lo);
    return 1;
}

s32 DisplayObjectStream_ConfigureRotation(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    int high;
    int low;
    object->flags |= GsROTOFF;
    object->field_22 = data[0];
    object->field_4A = (signed char)data[1];
    high = data[3] << 8;
    low = data[2];
    object->current += 4;
    object->field_48 = high | low;
    return 1;
}
