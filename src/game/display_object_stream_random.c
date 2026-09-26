#include "../types.h"
#include "../psyq/rand.h"
#include "display_object_stream_state.h"

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
