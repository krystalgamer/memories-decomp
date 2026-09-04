#include "../types.h"

extern u8 D_800EA0E8[];
extern volatile u32 D_8009B0F4;
extern void func_800289BC(void);
extern u8 *func_80014EEC(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    void *arg4,
    s32 arg5,
    s32 arg6);

u8 *func_80029164(s32 slot, s32 value)
{
    u8 *base = D_800EA0E8;
    u8 *record = &base[slot * 64];
    u8 *object;

    *(u16 *)(record + 0x30) = value;
    object = func_80014EEC(
        0, 0, (value - 1) * 7 + 0x2D2, 7, func_800289BC, 0, 0);
    *(s32 *)(object + 0x38) = slot;
    D_8009B0F4 = *(s32 *)(object + 0x2C) | 0x10;
    return object;
}
