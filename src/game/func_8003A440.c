#include "../types.h"

extern void func_800428EC(u8 *, s8);

void func_8003A440(u8 **arg0, u32 arg1, s32 arg2)
{
    u8 *e;
    s8 c;
    u32 *w;
    s32 i;

    if (arg1 == 0) {
        for (i = 2; i >= 0; i--) {
            c = (s8)arg2;
            e = arg0[i];
            if (e != 0) {
                *(u32 *)(e + 4) = *(u32 *)(e + 4) & 0x8FFFFFFF;
                *(u32 *)(e + 4) = *(u32 *)(e + 4) | 0x40000000;
                func_800428EC(e, c);
                *(u32 *)(e + 0xC) = 0x808080;
                *(u16 *)(e + 0x42) -= 1;
            }
        }
    } else {
        for (i = 2; i >= 0; i--) {
            e = arg0[i];
            if (e != 0) {
                w = (u32 *)(e + 4);
                *w = (*(u32 *)(e + 4) & 0x8FFFFFFF) | arg1;
                func_800428EC(e, (s8)arg2);
                if (arg1 == 0x60000000) {
                    *(u16 *)(e + 0x42) = 0xFD;
                } else {
                    *(u16 *)(e + 0x42) += 1;
                }
            }
        }
    }
}
