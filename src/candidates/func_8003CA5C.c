/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/game_over.c.
 */
#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "../game/display_object_config.h"
#include "../game/rand_get_interval.h"
#include "../game/display_object.h"
#include "../game/display_object_api.h"
#include "../game/fade.h"
#include "../game/graphics_constants.h"
#include "../game/input.h"
#include "../game/display_object_helpers.h"
#include "../game/sound.h"
#include "../game/display_object_config.h"
#include "../game/game_over.h"
#include "../unmatched.h"

/* The retail tail load uses an absolute, self-clobbering v1 address lifetime. */

s32 func_8003CA5C(void)
{
    DisplayObject *p;
    DisplayObjectVelocity *motion;
    s16 value;
    register u32 tail_bits __asm__("$3");

    p = (DisplayObject *)D_8009B378;
    motion = (DisplayObjectVelocity *)p;
    DisplayObject_StepPositionX(motion);
    value = p->field_5A;
    if (value == 0) {
        switch (p->field_6C) {
        case 0:
            func_80040410((DisplayObjectConfig *)p, 1);
            p->field_6C = 1;
            motion->velocity_x = -0xC0;
            break;
        case 1:
            DisplayObject_ResetVelocity(motion);
            func_80040410((DisplayObjectConfig *)p, 2);
            p->field_6C = 2;
            break;
        case 2:
            func_80040410((DisplayObjectConfig *)p, 0);
            p->field_6C = 0;
            if (motion->x < -0x2F) {
                motion->x = 0x160;
                motion->y = Rand_GetInterval(0xB0) + 0x30;
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
