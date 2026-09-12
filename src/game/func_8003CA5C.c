#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "display_object_config.h"
#include "rand_get_interval.h"
#include "display_object.h"
#include "display_object_api.h"
#include "fade.h"
#include "input.h"
#include "display_object_helpers.h"
#include "game_over.h"
#include "../unmatched.h"

s32 func_8003CA5C(void)
{
    DisplayObject *p;
    DisplayObjectVelocity *motion;
    s16 value;
    u32 tail_bits;

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

        if (gFade_State.flags & FADE_FLAG_ACTIVE) {
            return result;
        }

        tail_bits = gInput_wPad1Pressed;
        tail_bits &= PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK;
        if (tail_bits) {
            result = 0;
            /* Distinct exits keep the masked input live across result = 0. */
            if (tail_bits & PAD_BUTTON_CANCEL) {
                return result;
            } else {
                return result;
            }
        }
        return result;
    }
}
