#include "../../types.h"

extern s32 D_8016D428;

void func_8016A00C(u8 *a)
{
    s32 value = D_8016D428;

    *(s16 *)(a + 0x1A) = 0x63;
    *(s16 *)(a + 0x18) = value * 16 + 0xA3;
}
