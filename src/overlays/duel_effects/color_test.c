#include "../../types.h"
#include "utility_helpers.h"

s32 func_8014D378(const u8 *color)
{
    return color[0] == 0 && color[1] == 0 && color[2] == 0;
}

s32 func_8014D3AC(const u8 *color)
{
    return color[0] != 0 || color[1] != 0 || color[2] != 0;
}
