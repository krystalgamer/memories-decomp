#include "../../types.h"
#include "../display_object_stream_state.h"

s32 DisplayObjectStream_Stop(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_5A = 0;
    return -1;
}
