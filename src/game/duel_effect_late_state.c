#include "../types.h"

extern u8 D_8009B3C1;
extern u8 D_8009B3EB;

extern void func_8003E854(void);

void func_8003EE90(void)
{
    if ((D_8009B3C1 & 0x80) == 0) {
        D_8009B3C1 |= 0x80;
        D_8009B3EB = 0;
    }
    func_8003E854();
}

void func_8003EEC8(void)
{
}
