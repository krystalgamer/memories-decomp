#include "../types.h"

typedef struct {
    u8 pad_00[0x31];
    u8 field_31;
    u8 field_32;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x06];
    u16 field_3E;
} DisplayEffectState;

extern s32 rand(void);

s32 func_80039F1C(DisplayEffectState *object)
{
    u8 state = object->state;

    if ((state & 0x80) == 0) {
        object->state = state | 0x80;
        return 0;
    }
    return 1;
}

void func_80039F44(DisplayEffectState *object)
{
    object->field_34 = 0x68;
    object->field_32 = 0;
    object->state = 0;
    object->field_31 = 0;
    object->field_36 = 0xB2;
    object->field_3E = (rand() & 0xFF) + 0x3C;
}
