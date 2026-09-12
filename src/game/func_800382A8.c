#include "../types.h"
#include "duel_effect_command.h"

/* Inlining keeps the stream value and channel in independent live ranges. */
static __inline__ u32 read_operand(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];
    u8 *cursor = *stream;
    u32 value = *cursor++;

    *stream = cursor;
    return value;
}

void func_800382A8(u8 *argument)
{
    DuelEffectChannel *object = (DuelEffectChannel *)argument;
    u32 value;

    object->flags_34 &= 0xFEFF;
    value = read_operand(object);
    switch (value) {
    case 1:
        object->field_5A = 8;
        object->field_5B = 8;
        break;
    case 2:
        object->field_5A = 8;
        object->field_5B = 12;
        break;
    }
    if (value == 1)
        object->flags_34 |= 0x100;
}
