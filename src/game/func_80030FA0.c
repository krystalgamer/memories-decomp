#include "../types.h"

extern u8 D_80090D68[];
extern u8 D_8009B26C __attribute__((section(".data")));
extern u8 D_8009B2B2;
extern s8 D_8009B2F1;
extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));

void func_80030FA0(void)
{
    u8 *p = D_80090D68;
    s32 i = D_8009B2F1;
    u8 v;

    gFreeDuel_bReturnFlags = 0;
    v = p[i];
    D_8009B2B2 = 0;
    D_8009B26C = v;
}
