#include "../../types.h"
#define D_8009B118_IS_POINTER_IN_DATA
#include "../../unmatched.h"
#include "../../game/graphics_frame.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_helpers.h"
#include "../../game/display_asset_banks.h"
#include "../../game/sound.h"
#include "helpers.h"

void func_801686AC(s32 mode)
{
    RECT *rect;
    u8 *buffer;
    s32 index;
    DisplayObject *object;
    u32 music;

    buffer = D_8009B118;
    D_80169052 = mode;
    D_80169140 = D_8009C02B;
    rect = &D_800E9D70[1];
    rect->x = 0x290;
    rect->y = 0;
    rect->w = 0x30;
    rect->h = 0x10;
    StoreImage(rect, (u32 *)(buffer + 0x2000));
    DrawSync(0);
    {
        RECT *first = rect - 1;

        first->x = 0x290;
        first->y = 0xC0;
        first->w = 0x30;
        first->h = 0x10;
        StoreImage(first, (u32 *)D_8009B118);
    }
    DrawSync(0);
    buffer = D_8009B118;
    index = 0x60;
    while (buffer[index] == buffer[index + 0x2000]) {
        if (++index >= 0x5A0) {
            D_80169140 = D_8009B118[0];
            break;
        }
    }
    if (mode != 0) {
        func_801686A4(D_80169140);
        D_801691FC = 2;
        music = 0x7370;
    } else {
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, D_80169140, 2, 0x10, 0x100, D_801AF000);
        DisplayObject_SetDepthOffset(object, -5);
        object->flags |= 0x28;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, D_80169140, 3, 0x10, 0x100, D_801AF000);
        DisplayObject_SetDepthOffset(object, -4);
        D_80169074 = (DisplayObjectConfig *)object;
        object->flags |= 0x28;
        D_80169134 = 0;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, D_80169140, 4, 0x10, 0x100, D_801AF000);
        DisplayObject_SetDepthOffset(object, -4);
        object->flags |= 0x28;
        D_80169138 = (DisplayObjectConfig *)object;
        D_80169070 = gSD_bOutputType;
        if (D_80169070 < 0) {
            D_80169070 = 0;
        }

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, D_80169140, D_80169070, 0x10, 0x100, D_801AF000);
        DisplayObject_SetDepthOffset(object, -4);
        D_80169078 = (DisplayObjectConfig *)object;
        object->flags |= 0x28;
        D_801691FC = 2;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 20, 60, 3, 0, 2, 0xB, 0x20C);
        D_8016913C = object;
        object->flags |= 0x28;
        func_80168048(D_80169134);
        D_80169144 = 0;
        music = 0x7350;
    }
    SD_BGMPlay(music);
}
