#include "../../types.h"
#include "drawing_helpers.h"

u16 func_80156AD4(s16 value)
{
    u16 count = 0;
    while (__builtin_abs(value) > 0) {
        value /= 10;
        count++;
    }
    return count;
}

void func_80156B40(u16 value, u16 *digits)
{
    s16 original = value;
    s32 i;
    for (i = 0; i < func_80156AD4(original); i++) {
        digits[i] = value / (s16)func_8014F524(10, func_80156AD4(original) - (i + 1));
        value -= digits[i] * (s16)func_8014F524(10, func_80156AD4(original) - (i + 1));
    }
}
