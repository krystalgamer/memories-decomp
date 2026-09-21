#include "../types.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "duel_effect_command.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_80038498(DuelEffectChannel *object)
{
    u8 **slot =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_58];
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    object->field_54 = w;
}
