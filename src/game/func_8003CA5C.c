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

s32 func_8003CA5C(void)
{
    DisplayObject *p;
    DisplayObjectVelocity *motion;
    s16 value;

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
    if (!(gFade_State.flags & FADE_FLAG_ACTIVE) &&
        (gInput_wPad1Pressed &
         (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK))) {
        return 0;
    }
    return 1;
}
