#include "../types.h"

extern volatile u16 D_8009AF74[4] __attribute__((section(".sdata")));

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
