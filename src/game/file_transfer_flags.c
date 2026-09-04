#include "../types.h"

extern volatile u32 D_8009B0F4;
extern volatile u16 D_8009B112;
extern s32 D_8009B134;

void func_80015010(void);

void func_80014FA4(void)
{
    s32 value;

    if ((D_8009B0F4 & 0x2000030) | D_8009B134) {
        value = 0x80;
        if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
            func_80015010();
        }
        D_8009B134 = value;
    }
}

void func_80015010(void)
{
    D_8009B112 &= 0x3FFC;
    D_8009B112 |= 2;
}

void func_80015038(void)
{
    if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }
}
