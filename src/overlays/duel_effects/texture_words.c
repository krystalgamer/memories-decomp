#include "../../types.h"
#include "utility_helpers.h"

s32 func_8014F524(s16 base, u16 exponent)
{
    s32 result = 1;
    s32 i;

    for (i = 0; i < exponent; i++) {
        result *= base;
    }
    return (s16)result;
}

void func_8014F564(u16 *output, GsIMAGE *image, s32 mode)
{
    output[0] = ((*(u16 *)&image->pmode & 3) << 7) |
                ((mode & 3) << 5) |
                (((s32)(*(u16 *)&image->py & 0x100) << 16) >> 20) |
                ((*(u16 *)&image->px & 0x3FF) >> 6) |
                ((*(u16 *)&image->py & 0x200) << 2);
    output[1] = (*(u16 *)&image->cy << 6) |
                ((*(u16 *)&image->cx >> 4) & 0x3F);
}

void func_8014F5D0(u16 *output, GsIMAGE *image)
{
    output[0] = ((*(u16 *)&image->pmode & 3) << 7) | 0x3E;
    output[1] = (*(u16 *)&image->cy << 6) |
                ((*(u16 *)&image->cx >> 4) & 0x3F);
}
