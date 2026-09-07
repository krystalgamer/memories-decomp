#include "../types.h"
#include "color_constants.h"

typedef struct { u8 r, g, b; } Color;
extern void func_8005ABA0(Color *, s32, u32, u32, s32);
s32 func_8005B054(s32 value, u32 a, u32 b)
{
    Color color;
    func_8005ABA0(
        &color, value, a & 0xFFFF, b & 0xFFFF, COLOR_BGR555_CHANNEL_MASK);
    return (color.r & COLOR_BGR555_CHANNEL_MASK) |
           ((color.g & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_GREEN_SHIFT) |
           ((color.b & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_BLUE_SHIFT);
}
