#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_stream_state.h"

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
