#include "../types.h"
#include "duel_effect.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_80038334(DuelEffectChannel *object)
{
    /* Separate lifetimes preserve allocation across the two stream reads. */
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5A = value;
    }
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->field_5B = value;
    }
}
