#include "../types.h"

extern s32 D_8009B30C __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern u8 D_800F2848[];

void func_800220B8(void) {
    u8 *b;
    u8 *c;
    s32 a;
    s32 v;
    s32 x;
    s32 y;

    if ((gInput_wPad1Pressed & 0x100) != 0) {
        D_8009B30C = D_8009B30C ^ 1;
    }

    b = D_800F2848;

    if ((gInput_wPad1Repeat & 0xC) != 0) {
        a = 2;
        if ((gInput_wPad1Held & 0x40) != 0) {
            a = 0x10;
        }
        v = *(s16 *)D_800F2848 + a;
        if ((gInput_wPad1Repeat & 4) != 0) {
            v -= a * 2;
        }
        *(s16 *)D_800F2848 = v;
        func_8001352C();
    }

    if ((gInput_wPad1Repeat & 0xF000) != 0) {
        if ((gInput_wPad1Held & 0x10) != 0) {
            a = 2;
            if ((gInput_wPad1Held & 0x40) != 0) {
                a = 0x10;
            }
            if ((gInput_wPad1Repeat & 0x2000) != 0) {
                *(s32 *)(b + 0x1C) = *(s32 *)(b + 0x1C) + a;
            }
            if ((gInput_wPad1Repeat & 0x4000) != 0) {
                *(s32 *)(b + 0x24) = *(s32 *)(b + 0x24) - a;
            }
            if ((gInput_wPad1Repeat & 0x8000) != 0) {
                *(s32 *)(b + 0x1C) = *(s32 *)(b + 0x1C) - a;
            }
            if ((gInput_wPad1Repeat & 0x1000) != 0) {
                *(s32 *)(b + 0x24) = *(s32 *)(b + 0x24) + a;
            }
        } else {
            v = 0x20;
            if ((gInput_wPad1Held & v) != 0) {
                v = 0x80;
            }
            y = *(s16 *)(b + 2);
            x = *(s16 *)(b + 4);
            if ((gInput_wPad1Repeat & 0x1000) != 0) {
                x += v;
            }
            if ((gInput_wPad1Repeat & 0x4000) != 0) {
                x -= v;
            }
            if ((gInput_wPad1Repeat & 0x2000) != 0) {
                y -= v;
            }
            if ((gInput_wPad1Repeat & 0x8000) != 0) {
                y += v;
            }
            c = D_800F2848;
            *(s16 *)(c + 2) = y;
            *(s16 *)(c + 4) = x;
        }
        func_8001352C();
    }
}
