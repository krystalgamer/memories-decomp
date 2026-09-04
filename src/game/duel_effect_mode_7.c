#include "../types.h"

extern u8 D_8009B2F8;
extern u8 D_8009B26C[];

void func_80033C90(void)
{
    D_8009B2F8 = 0;
    D_8009B26C[0] = 7;
}

void func_80033CA8(void)
{
    D_8009B2F8 = 0x80;
    D_8009B26C[0] = 7;
}
