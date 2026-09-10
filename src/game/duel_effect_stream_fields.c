#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_stream_fields.h"
#include "func_80036D3C.h"

void func_80038334(DuelEffectChannel *object)
{
    register u8 **stream __asm__("$3");
    register u8 *current __asm__("$2");
    register u8 value __asm__("$5");

    stream = &((u8 **)object)[object->stream_58];
    current = *stream;
    value = *current++;
    *stream = current;
    object->field_5A = value;
    stream = &((u8 **)object)[object->stream_58];
    current = *stream;
    value = *current++;
    *stream = current;
    object->field_5B = value;
}

void func_80038388(DuelEffectChannel *object)
{
    object->field_38 = func_80036D3C((u8 *)object);
}

void func_800383B0(DuelEffectChannel *object)
{
    object->field_60 = 0;
    object->field_61 = func_80036D3C((u8 *)object);
}
