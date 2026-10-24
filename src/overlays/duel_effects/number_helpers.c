#include "../../types.h"
#include "drawing_helpers.h"
#include "number_semantic_views.h"

u16 DuelEffect_CountDecimalDigitsS16(s16 value)
{
    u16 count = 0;
    while (__builtin_abs(value) > 0) {
        value /= 10;
        count++;
    }
    return count;
}

void DuelEffect_WriteNumberDigits(u16 value, u16 *digits)
{
    s16 original = value;
    s32 i;
    for (i = 0; i < DuelEffect_CountDecimalDigitsS16(original); i++) {
        digits[i] = value / (s16)DuelEffect_PowS16(10, DuelEffect_CountDecimalDigitsS16(original) - (i + 1));
        value -= digits[i] * (s16)DuelEffect_PowS16(10, DuelEffect_CountDecimalDigitsS16(original) - (i + 1));
    }
}
