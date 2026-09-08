#include "../types.h"

#include "fade.h"

void func_800156B8(s32 arg0)
{
    u8 *p = (u8 *)&gFade_State;
    s32 i;

    for (i = 0x1D; i >= 0; i--) {
        *(p + i + 0xA) = arg0;
    }
}
