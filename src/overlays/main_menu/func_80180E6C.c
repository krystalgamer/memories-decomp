#include "../../types.h"

extern u8 D_801AF800[];
extern void func_80180F50(void *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_80042918(void *);
extern void func_800428EC(void *, s32);

void func_80180E6C(u8 *source)
{
    u8 *object;

    object = func_800400AC(func_8004002C(), 2);
    if (object != 0) {
        func_800428A8(object, *(s16 *)(source + 0x30), *(s16 *)(source + 0x32), 0,
                      0, source[0x69], 0x18, 0, D_801AF800);
        *(s32 *)(object + 4) |= 0x51000000;
        *(u16 *)(object + 8) |= 0x48;
        func_80042918(object);
        func_800428EC(object, (s8)(-source[0x60]));
        *(void **)(object + 0x24) = func_80180F50;
        object[0xC] = source[0xC];
        object[0xD] = source[0xD];
        object[0xE] = source[0xE];
    }
}
