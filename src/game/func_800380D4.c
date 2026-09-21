#include "../types.h"
#include "duel_effect.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_800380D4(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->field_38 = 0;
    stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->field_3A += (s8)value;
}
