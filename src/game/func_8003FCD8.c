#include "../types.h"
#include "func_8003F8D4.h"
#include "func_8003FCD8.h"

extern u8 D_8009B3ED;
extern u8 D_8009B3C0;

s32 func_8003FCD8(void)
{
    if ((D_8009B3ED & 0x80) == 0) {
        D_8009B3ED |= 0x80;
        D_8009B3C0 = 0x29;
    }
    return func_8003F8D4();
}
