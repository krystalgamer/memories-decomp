#include "../types.h"
#include "mem_card.h"
#include "func_8003F8D4.h"
#include "func_8003FCD8.h"

s32 func_8003FCD8(void)
{
    if ((D_8009B3ED & 0x80) == 0) {
        D_8009B3ED |= 0x80;
        D_8009B3C0 = 0x29;
    }
    return func_8003F8D4();
}
