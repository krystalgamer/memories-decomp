#include "../../types.h"
#include "color_semantic_views.h"
#include "utility_helpers.h"

s32 DuelEffect_IsColorBlack(const u8 *color)
{
    return color[0] == 0 && color[1] == 0 && color[2] == 0;
}

s32 DuelEffect_HasNonzeroColor(const u8 *color)
{
    return color[0] != 0 || color[1] != 0 || color[2] != 0;
}
