#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_command.h"
#include "func_80036C14.h"

void func_80038024(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= 0x80;
    func_80036C14(object, value);
    object->flags_34 &= 0xFF7F;
    object->field_38 += 0x10;
}

extern u8 D_8009B344;

void func_80038070(DuelEffectChannel *object)
{
    func_80038024(object, D_8009B344);
}

void func_80038094(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];

    func_80038024(object, *(*stream)++);
}

void func_800380D4(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->field_38 = 0;
    stream = &((u8 **)object)[object->stream_58];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->field_3A += (s8)value;
}

void func_80038110(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->field_38 += value;
}
