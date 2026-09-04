#include "../types.h"

extern u8 D_8009B3C1;
extern u8 D_8009B3EB;
extern u16 D_8009B3FA;

extern void func_8003E490(void);

void func_8003E7D4(void)
{
    if ((D_8009B3C1 & 0x80) == 0) {
        D_8009B3C1 |= 0x80;
        D_8009B3EB = 0;
    }
    func_8003E490();
}

void func_8003E80C(void)
{
    if (!(D_8009B3C1 & 0x80)) {
        D_8009B3C1 |= 0x80;
        D_8009B3EB = 1;
        D_8009B3FA |= 0x200;
    }
    func_8003E490();
}
