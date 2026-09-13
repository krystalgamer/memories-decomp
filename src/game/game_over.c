#include "../types.h"
#include "display_object_config.h"
#include "rand_get_interval.h"
#include "display_object.h"
#include "display_object_api.h"
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
    object = func_800400AC(func_8004002C(), 3);
    func_80040510(
        (DisplayObjectConfigView *)object, 0, 0, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 0, 16, 0, 240
    );
    object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    object = func_800400AC(func_8004002C(), 2);
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
