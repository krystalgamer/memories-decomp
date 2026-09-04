#include "../types.h"

extern u8 *D_800EB184;

void func_80030090(void)
{
    D_800EB184[12] = D_800EB184[13] = D_800EB184[14] = 0x40;
}

void func_800300AC(void)
{
    D_800EB184[12] = D_800EB184[13] = D_800EB184[14] = 0x80;
}
