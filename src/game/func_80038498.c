#include "../types.h"

extern u8 gText_abColorSlots[];

void func_80038498(u8 *arg0)
{
    u8 **slot = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    arg0[0x54] = w;
}
