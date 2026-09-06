#include "../types.h"
#include "input.h"

extern u16 D_8009B098;
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern s16 D_800E9D28[];

extern void func_80012D4C(void);
extern void FntFlush(s32);
extern void Input_ResetPads(void);

void func_80013360(void)
{
    register s16 *p asm("$16");
    s32 step;

    p = D_800E9D28;
    p[0] = 0;
    p[1] = 0;
    D_8009B098 |= 0x2000;
    goto poll;
adjust:
    if (gInput_wPad1Held & PAD_DIRECTION_MASK) {
        step = 2;
        if (gInput_wPad1Held & PAD_BUTTON_CROSS) {
            step = 4;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_RIGHT) {
            p[0] += step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_LEFT) {
            p[0] -= step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_UP) {
            p[1] -= step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_DOWN) {
            p[1] += step;
        }
    }
    FntFlush(-1);
poll:
    func_80012D4C();
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto adjust;
    }
    D_8009B098 &= 0xDFFF;
    Input_ResetPads();
}
