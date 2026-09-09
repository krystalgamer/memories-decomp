#include "../types.h"
#include "mem_card.h"
#include "func_800440B4.h"

extern s8 D_8009B43E;
extern char D_8009B43C;
extern char D_8009B44F;
extern char D_8009B43D;

s32 func_800440B4(s32 first, s32 second)
{
    if (D_8009B43E >= 0)
        return 0;
    D_8009B43C = 10;
    D_8009B437 = first;
    D_8009B43E = second;
    D_8009B44F = 0;
    D_8009B43D = 0;
    gMemCard_nIOResult = -1;
    return 1;
}
