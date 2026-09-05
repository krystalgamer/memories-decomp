#include "../types.h"

extern u8 * volatile g_SDValue;
extern void func_800473CC(s32);
extern void func_80045208(s32, s32);
extern void func_80049230(s32, s32);

void func_8004733C(s32 arg0, s32 arg1)
{
    register s32 v asm("s1") = arg0;

    if ((g_SDValue[0x4A] & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_800473CC(0x7000);
        func_80045208(v & 0xFFFF, (s16)arg1);
    } else {
        register u32 masked asm("v0") = (u32)(v & 0xFFFF);

        if (masked >= 0x7000) {
            arg0 -= 0x7000;
        }
        func_80049230((s16)arg0, (s16)arg1);
    }
}
