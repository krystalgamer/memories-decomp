#include "../../types.h"

s32 func_80181EEC(s32 value)
{
    s32 digits = 0;
    s32 next;

    do {
        digits++;
        next = value / 10;
        value = next;
    } while (next != 0);

    return digits;
}
