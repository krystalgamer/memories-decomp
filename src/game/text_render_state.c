#include "../types.h"

extern u32 D_8009B300;
extern u32 D_8009B304;
extern u32 D_8009B30C;
extern u32 D_8009B310;
extern u32 D_8009B314;

void func_80035668(u32 value)
{
    D_8009B30C = value;
    D_8009B300 = 0x808080;
}

void func_80035680(u32 value)
{
    D_8009B314 = 0;
    D_8009B310 = value;
    D_8009B304 = value;
    D_8009B30C |= 4;
}
