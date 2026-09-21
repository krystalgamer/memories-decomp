#include "../types.h"
#include "display_object_stream_state.h"

s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_58 = 0;
    return 1;
}
