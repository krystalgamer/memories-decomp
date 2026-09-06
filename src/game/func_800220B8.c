#include "../types.h"
#include "input.h"

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

    if ((gInput_wPad1Pressed & PAD_BUTTON_SELECT) != 0) {
        D_8009B30C = D_8009B30C ^ 1;
    }

    b = D_800F2848;

    if ((gInput_wPad1Repeat & PAD_BUTTON_L1_R1_MASK) != 0) {
        a = 2;
        if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
            a = 0x10;
        }
        v = *(s16 *)D_800F2848 + a;
        if ((gInput_wPad1Repeat & PAD_BUTTON_L1) != 0) {
            v -= a * 2;
        }
        *(s16 *)D_800F2848 = v;
        func_8001352C();
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_TRIANGLE) != 0) {
            a = 2;
            if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                a = 0x10;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                *(s32 *)(b + 0x1C) = *(s32 *)(b + 0x1C) + a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
                *(s32 *)(b + 0x24) = *(s32 *)(b + 0x24) - a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_LEFT) != 0) {
                *(s32 *)(b + 0x1C) = *(s32 *)(b + 0x1C) - a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
                *(s32 *)(b + 0x24) = *(s32 *)(b + 0x24) + a;
            }
        } else {
            v = 0x20;
            if ((gInput_wPad1Held & PAD_BUTTON_CIRCLE) != 0) {
                v = 0x80;
            }
            y = *(s16 *)(b + 2);
            x = *(s16 *)(b + 4);
            if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
                x += v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
                x -= v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                y -= v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_LEFT) != 0) {
                y += v;
            }
            c = D_800F2848;
            *(s16 *)(c + 2) = y;
            *(s16 *)(c + 4) = x;
        }
        func_8001352C();
    }
}
