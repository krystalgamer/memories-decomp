#include "../../types.h"
#include "color_helpers.h"

void func_80153F28(u8 *color, u16 step)
{
    if (color[0] > step + 1) {
        color[0] -= step;
    } else {
        color[0] = 0;
    }
    if (color[1] > step + 1) {
        color[1] -= step;
    } else {
        color[1] = 0;
    }
    if (color[2] > step + 1) {
        color[2] -= step;
    } else {
        color[2] = 0;
    }
}

s32 func_80153F98(u8 *color, u8 red, u8 green, u8 blue, u16 step)
{
    if (color[0] < (u16)(red + 1) - step) {
        color[0] += step;
    } else {
        color[0] = red;
    }
    if (color[1] < (u16)(green + 1) - step) {
        color[1] += step;
    } else {
        color[1] = green;
    }
    if (color[2] < (u16)(blue + 1) - step) {
        color[2] += step;
    } else {
        color[2] = blue;
    }
    if (color[0] == red && color[1] == green && color[2] == blue) {
        return 1;
    }
    return 0;
}

void func_8015405C(u16 high, u16 middle, u16 low)
{
    D_8009B300 = ((u8)high << 16) | ((u8)middle << 8) | (u8)low;
}
