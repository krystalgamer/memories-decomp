#include "../types.h"
#include "display_object_stream_state.h"

/* Entries 0 through 4 of tent_DisplayObjectStreamCommandTable. */
s32 DisplayObjectStream_Stop(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_5A = 0;
    return -1;
}

s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_58 = 0;
    return 1;
}

s32 DisplayObjectStream_Noop(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    return 1;
}

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
