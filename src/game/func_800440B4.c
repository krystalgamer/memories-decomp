#include "../types.h"

extern signed char D_8009B43E;
extern char D_8009B43C;
extern char D_8009B437;
extern char D_8009B44F;
extern char D_8009B43D;
extern int gMemCard_nIOResult;

int func_800440B4(int first, int second)
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
