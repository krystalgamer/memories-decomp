#include "../types.h"
#include "display_object_layout.h"

extern u8 D_800EA1E8[];
extern s32 func_8004002C(void);
extern u8 *func_800400AC();
extern void func_800404CC();
extern void func_8004293C();
extern void func_800428EC();

void func_80029590(void)
{
    s32 i = 0;
    u8 **slot = (u8 **)D_800EA1E8;
    for (; i < 8; i++, slot++) {
        u8 *object = func_800400AC(func_8004002C(), 2);
        func_800404CC(object, 8, (i >> 1) * 178 + 8,
                      0, 3, i, 25, 262);
        if (i & 1)
            *(s16 *)(object + 48) = 168;
        *(s32 *)(object + 4) |= 0x1000000;
        *(u16 *)(object + 8) &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_8004293C(object);
        func_800428EC(object, 0);
        slot[9] = object;
    }
}
