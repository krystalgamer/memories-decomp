#include "../types.h"

extern unsigned char D_801AB00C[];
extern unsigned char D_8009B1D5;
extern void func_80027DF8(void *, int);

void func_80028220(void)
{
    func_80027DF8(D_801AB00C, D_8009B1D5);
    func_80027DF8(D_801AB00C + 0x294, D_8009B1D5 ^ 1);
}

int func_80028260(int value)
{
    if (value & 0x80) {
        return (value & 0x7F) + 0xF;
    }
    return value;
}
