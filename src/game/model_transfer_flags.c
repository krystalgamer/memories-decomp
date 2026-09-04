#include "../types.h"

extern u8 D_8009B07B;
extern u8 D_8009B07C;

s32 func_8005F174(void)
{
    return D_8009B07B;
}

void func_8005F180(s32 value)
{
    D_8009B07B = value;
}

s32 func_8005F18C(void)
{
    return D_8009B07C;
}

void func_8005F198(s32 value)
{
    D_8009B07C = value;
}
