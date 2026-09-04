#include "../types.h"

extern u8 * volatile g_SDValue;
extern void func_80045334(s32);
extern void func_80049138(s32, s32);

void func_800472A8(s32 arg0)
{
    register s32 v asm("v1") = arg0;

    if ((g_SDValue[0x4A] & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_80045334(v & 0xFFFF);
    } else {
        register u32 masked asm("v0") = (u32)(v & 0xFFFF);

        if (masked >= 0x7000) {
            arg0 -= 0x7000;
        }
        func_80049138((s16)arg0, 1);
    }
}
