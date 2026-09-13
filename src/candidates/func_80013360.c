/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_services.c.
 */
#define D_8009B0A3_IS_VOLATILE_SCALAR
#define D_8009B142_IN_DATA_VOLATILE
#define GRAPHICS_DRAW_ENV_IS_VOLATILE
/* func_80013360 re-reads the pad word on each path and needs it out of
   small data; see the arms in input.h. */
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
/* The fade target colour is part of the same init block; see the arm in
   fade.h. */
#define D_8009B14A_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "../game/fade.h"
#include "../unmatched.h"
#include "../game/func_800136D4.h"
#include "../game/func_80041340.h"
#include "../game/graphics_constants.h"
#include "../game/graphics_frame.h"
#include "../game/main_frame.h"
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#include "../game/input.h"
#include "../game/rand_constants.h"
#include "../game/main_services.h"

/* The resident system layer: the per-frame service pump, the boot-time
   graphics and input start-up that installs it, the pad-driven screen-offset
   adjustment loop, and the reset of the callback registry the pump walks.
   The four are contiguous and are the only run in the region built with
   gcc_2_8_1_g8_split - their neighbours on both sides use other profiles -
   and the pump and the reset share the D_800E9DB0 slots and D_8009B0B8. */

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

