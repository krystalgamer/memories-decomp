#include "../types.h"

extern unsigned char D_8009B3ED;
extern u8 D_8009B3C0;
extern void func_8003F8D4(void);

void func_8003FCD8(void)
{
    if ((D_8009B3ED & 0x80) == 0) {
        D_8009B3ED |= 0x80;
        D_8009B3C0 = 0x29;
    }
    func_8003F8D4();
}
