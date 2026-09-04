#include "../types.h"

typedef struct {
    u8 pad_00[0x14];
    s16 field_14;
    s8 field_16;
    u8 field_17;
} DisplayObjectAnimationState;

extern volatile u16 D_8009AF74[4] __attribute__((section(".sdata")));
extern u16 D_8009AF76;
extern u16 D_8009AF7A;

s32 func_800428EC(u8 *object, s8 value)
{
    u32 index = object[0x17];
    volatile u16 *table = D_8009AF74;
    s32 result;

    object[0x16] = value;
    result = table[index] - value;
    *(u16 *)(object + 0x14) = result;
    return result;
}

void func_80042918(DisplayObjectAnimationState *object)
{
    object->field_17 = 1;
    object->field_14 = D_8009AF76 - object->field_16;
}

void func_8004293C(DisplayObjectAnimationState *object)
{
    object->field_17 = 3;
    object->field_14 = D_8009AF7A - object->field_16;
}
