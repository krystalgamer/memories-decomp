#include "../types.h"

extern u16 D_8009B220;

int func_80024E24(void)
{
    u16 value = D_8009B220;

    if (!(value & 0x80)) {
        D_8009B220 = value | 0x80;
        return 0;
    }
    return 1;
}

void func_80024E4C(void)
{
    D_8009B220 = 0;
}
