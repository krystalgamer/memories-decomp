#include "../types.h"

extern s32 func_8004BAE4(u8 *);

s32 func_8004BC2C(u8 *input)
{
    s32 first = func_8004BAE4(input);
    s32 second = func_8004BAE4(input);
    s32 third = func_8004BAE4(input);
    s32 fourth = func_8004BAE4(input);

    return (fourth & 0xFF) + ((third & 0xFF) << 8) +
           ((second & 0xFF) << 16) + (first << 24);
}

s32 func_8004BCA8(u8 *input)
{
    s32 high = func_8004BAE4(input);
    s32 low = func_8004BAE4(input);

    return (low & 0xFF) | ((high & 0xFF) << 8);
}
