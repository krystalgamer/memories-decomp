#include "../types.h"

extern u8 D_800F3938[];
extern u8 *func_8005F1A4(s32 index);

s32 func_8005A618(s32 index)
{
    s32 offset = index * 3616;
    u8 *coefficient;
    s32 value;
    s32 record;
    s32 biased;

    record = offset + (s32)D_800F3938;
    coefficient = func_8005F1A4(
        *(u8 *)(record + (D_800F3938 + offset)[0x106] + 0xA) & 0x1F
    );

    value = *(s16 *)(coefficient + 2);
    if (index <= 0) {
        biased = value + 0x1C00;
        return biased % 4096;
    }

    biased = value + 0x1400;
    return biased % 4096;
}
