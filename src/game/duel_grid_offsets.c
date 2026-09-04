#include "../types.h"

s32 func_8002C400(s32 value)
{
    if (value >= 10) {
        return (value % 10) * 14 + 174;
    }
    return (value % 10) * 14 + 14;
}

s32 func_8002C484(s32 value)
{
    s32 quotient = value / 10;

    return quotient * 178 + (value - quotient * 10) * 16 + 14;
}
