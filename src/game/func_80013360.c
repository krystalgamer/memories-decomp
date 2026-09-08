#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "graphics_frame.h"
#include "input.h"

extern u16 D_8009B098;
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern void func_80012D4C(void);

void func_80013360(void)
{
    register s16 *p asm("$16");
    s32 step;

    p = (s16 *)&gGraphics_DispEnv;
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
