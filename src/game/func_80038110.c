#include "../types.h"
#include "duel_effect.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_80038110(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->field_38 += value;
}
