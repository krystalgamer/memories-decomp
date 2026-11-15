#include "../../types.h"
#include "color_semantic_views.h"
#include "color_helpers.h"

void DuelEffect_DarkenColor(CVECTOR *color, u16 step)
{
    if (color->r > step + 1) {
        color->r -= step;
    } else {
        color->r = 0;
    }
    if (color->g > step + 1) {
        color->g -= step;
    } else {
        color->g = 0;
    }
    if (color->b > step + 1) {
        color->b -= step;
    } else {
        color->b = 0;
    }
}

s32 DuelEffect_StepColorToTarget(CVECTOR *color, u8 red, u8 green, u8 blue, u16 step)
{
    if (color->r < (u16)(red + 1) - step) {
        color->r += step;
    } else {
        color->r = red;
    }
    if (color->g < (u16)(green + 1) - step) {
        color->g += step;
    } else {
        color->g = green;
    }
    if (color->b < (u16)(blue + 1) - step) {
        color->b += step;
    } else {
        color->b = blue;
    }
    if (color->r == red && color->g == green && color->b == blue) {
        return 1;
    }
    return 0;
}

void DuelEffect_SetFogBaseColor(u16 high, u16 middle, u16 low)
{
    gModel_dwFogBaseColor = ((u8)high << 16) | ((u8)middle << 8) | (u8)low;
}
