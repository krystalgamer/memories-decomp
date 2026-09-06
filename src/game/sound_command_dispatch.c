#include "../types.h"

#define SDVALUE_CUSTOM_EXTERN
#include "sound.h"

extern SDValue * volatile g_SDValue;
extern void func_80045114(void);
extern void func_80045208(s32, s32);
extern void func_800490F0(s16, u8);
extern void func_80049108(s16, u8);
extern void func_80049230(s32, s32);
extern void func_80049230_s16(s32, s16) asm("func_80049230");

void func_8004733C(s32 arg0, s32 arg1);
void func_800473CC(u32 value);
void func_800473F0(u16 flags, s32 value);

void func_80047314(u32 value)
{
    func_8004733C(value & 0xFFFF, g_SDValue->field_164B);
}

void func_8004733C(s32 arg0, s32 arg1)
{
    register s32 v asm("s1") = arg0;

    if ((g_SDValue->flags_004A & 2) == 0) {
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

void func_800473CC(u32 value)
{
    func_800473F0(value & 0xFFFF, -32);
}

void func_800473F0(u16 flags, s32 value)
{
    if ((flags & 0x8000) != 0)
        func_80045114();
    else
        func_80049230_s16(-1, value);
}

void func_80047430(s32 value, s32 flag)
{
    func_80049108(value, flag);
}

void func_80047458(s32 value, s32 flag)
{
    func_800490F0(value, flag);
}
