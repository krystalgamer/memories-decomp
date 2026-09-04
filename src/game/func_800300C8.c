#include "../types.h"

struct LayoutSource {
    u8 pad0[0x3C];
    s16 base;
    s16 spacing;
    u16 row;
};

struct LayoutTarget {
    u8 pad0[0x28];
    s16 field28;
    s16 field2A;
    u8 pad2C[4];
    s16 field30;
    s16 field32;
    u8 pad34[4];
    s16 field38;
    s16 field3A;
    u8 pad3C[4];
    s16 field40;
    s16 field42;
};

extern struct LayoutSource D_800EB15C;
extern struct LayoutTarget *D_8009B2E4;
extern s8 D_8009B2F1;

void func_800300C8(void)
{
    struct LayoutSource *source = &D_800EB15C;
    s32 first = source->base;
    struct LayoutTarget *target = D_8009B2E4;
    s32 index = D_8009B2F1;
    s32 second;
    s32 third;

    if (index >= 10) {
        register s32 raw_spacing asm("$2") =
            (s32)((u32)(u16)source->spacing << 16);
        register s32 half_spacing asm("$3") = raw_spacing >> 16;

        raw_spacing = (s32)((u32)raw_spacing >> 31);
        half_spacing += raw_spacing;
        half_spacing >>= 1;
        first -= 16;
        first += half_spacing;
    }
    target->field38 = first;
    target->field28 = first;

    second = first + source->spacing / 2 - 16;
    target->field40 = second;
    target->field30 = second;

    {
        s32 quotient = index / 10;
        s8 remainder = (s8)(index - quotient * 10);
        third = source->row + remainder * 16;
    }
    target->field32 = third;
    target->field2A = third;
    third += 16;
    target->field42 = third;
    target->field3A = third;
}
