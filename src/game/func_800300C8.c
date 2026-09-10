#include "../types.h"
#include "func_800300C8.h"
#include "duel_effect.h"
#include "frontend_debug_tables.h"

extern struct LayoutTarget *D_8009B2E4;

void func_800300C8(void)
{
    struct LayoutSource *source = (struct LayoutSource *)&D_800EB15C;
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
