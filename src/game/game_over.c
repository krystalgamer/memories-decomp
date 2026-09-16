#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "display_object_config.h"
#include "rand_get_interval.h"
#include "display_object.h"
#include "display_object_core.h"
#include "fade.h"
#include "graphics_constants.h"
#include "input.h"
#include "display_object_helpers.h"
#include "sound.h"
#include "display_object_config.h"
#include "game_over.h"
#include "../unmatched.h"

void func_8003C950(void)
{
    DisplayObject *object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 3);
    DisplayObject_ConfigureScreenSprite(
        (DisplayObjectConfigView *)object, 0, 0, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 0, 16, 0, 240
    );
    object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800428A8(object, 448, 192, 0, 0, 0, 18, 1, D_801AF000);
    *(u8 *)&object->field_5E = 128;
    func_800428EC((u8 *)object, 4);
    object->field_6C = 0;
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
    D_8009B378 = (s32)object;
    SD_BGMPlay(0x7300);
    Fade_StartIn();
    gFade_State.step = 2;
}

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
            DisplayObject_SetResourceVariant((DisplayObjectConfig *)p, 1);
            p->field_6C = 1;
            motion->velocity_x = -0xC0;
            break;
        case 1:
            DisplayObject_ResetVelocity(motion);
            DisplayObject_SetResourceVariant((DisplayObjectConfig *)p, 2);
            p->field_6C = 2;
            break;
        case 2:
            DisplayObject_SetResourceVariant((DisplayObjectConfig *)p, 0);
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
            if (tail_bits & PAD_BUTTON_CANCEL) {
                return result;
            } else {
                return result;
            }
        }
        return result;
    }
}
