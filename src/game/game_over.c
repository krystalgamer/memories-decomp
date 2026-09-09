#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "display_object_config.h"
#include "rand_get_interval.h"
#include "display_object_api.h"
#include "fade.h"
#include "graphics_constants.h"
#include "input.h"
#include "display_object_helpers.h"
#include "sound.h"
#include "../unmatched.h"

extern s32 D_8009B378;
/* The retail tail load uses an absolute, self-clobbering v1 address lifetime. */
extern void func_80040410(u8 *, s32);

void func_8003C950(void)
{
    u8 *object;
    object = func_800400AC(func_8004002C(), 3);
    func_80040510(
        (DisplayObjectConfigView *)object, 0, 0, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 0, 16, 0, 240
    );
    *(s32 *)(object + 4) |= 0x1000000;
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 448, 192, 0, 0, 0, 18, 1, D_801AF000);
    object[94] = 128;
    func_800428EC(object, 4);
    object[108] = 0;
    *(u16 *)(object + 8) |= 40;
    func_800429D8(object);
    D_8009B378 = (s32)object;
    SD_BGMPlay(0x7300);
    Fade_StartIn();
    gFade_State.step = 2;
}

s32 func_8003CA5C(void)
{
    u8 *p;
    s16 value;
    register u32 tail_bits __asm__("$3");

    p = (u8 *)D_8009B378;
    func_80042A00(p);
    value = *(s16 *)(p + 0x5A);
    if (value == 0) {
        switch (p[0x6C]) {
        case 0:
            func_80040410(p, 1);
            p[0x6C] = 1;
            *(s16 *)(p + 0x36) = -0xC0;
            break;
        case 1:
            func_800429D8(p);
            func_80040410(p, 2);
            p[0x6C] = 2;
            break;
        case 2:
            func_80040410(p, 0);
            p[0x6C] = 0;
            if (*(s16 *)(p + 0x30) < -0x2F) {
                *(s16 *)(p + 0x30) = 0x160;
                *(s16 *)(p + 0x32) = Rand_GetInterval(0xB0) + 0x30;
            }
            break;
        }
    }
    {
        s32 result = 1;

        if (gFade_State.flags & 0x80) {
            return result;
        }

        tail_bits = gInput_wPad1Pressed;
        if (tail_bits & (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) {
            result = 0;
            /* Distinct exits preserve the retail branch-and-delay-slot shape. */
            if (D_8009B378 != 0) {
                return result;
            } else {
                return result;
            }
        }
        return result;
    }
}
