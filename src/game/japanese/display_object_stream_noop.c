#include "../../types.h"
#include "../display_object_stream_state.h"

s32 DisplayObjectStream_Noop(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    return 1;
}
