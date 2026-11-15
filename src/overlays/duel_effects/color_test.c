#include "../../types.h"
#include "color_semantic_views.h"
#include "utility_helpers.h"

s32 DuelEffect_IsColorBlack(const CVECTOR *color)
{
    return color->r == 0 && color->g == 0 && color->b == 0;
}

s32 DuelEffect_HasNonzeroColor(const CVECTOR *color)
{
    return color->r != 0 || color->g != 0 || color->b != 0;
}
