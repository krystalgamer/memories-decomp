#include "../../types.h"
#include "../../game/main.h"
#include "../../game/image_transfer_rects.h"
#include "../../game/display_object_system.h"
#include "../../game/display_asset_banks.h"
#include "../../game/sound.h"
#include "helpers.h"

void Options_Init(s32 mode)
{
    RECT *rect;
    u8 *buffer;
    s32 index;
    DisplayObject *object;
    u32 music;

    buffer = gFile_TransferBufferBase;
    gPalOptions_bMode = mode;
    gPalOptions_bLanguage = gText_bLanguageIndex;
    rect = &gGraphics_aImageTransferRects[1];
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
        StoreImage(first, (u32 *)gFile_TransferBufferBase);
    }
    DrawSync(0);
    buffer = gFile_TransferBufferBase;
    index = 0x60;
    while (buffer[index] == buffer[index + 0x2000]) {
        if (++index >= 0x5A0) {
            gPalOptions_bLanguage = gFile_TransferBufferBase[0];
            break;
        }
    }
    if (mode != 0) {
        PalOptions_NoOpLanguageHook(gPalOptions_bLanguage);
        gPalOptions_bState = 2;
        music = 0x7370;
    } else {
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 2, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -5);
        object->flags |= 0x28;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 3, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        gPalOptions_pLanguageSprite = (DisplayObjectConfig *)object;
        object->flags |= 0x28;
        gPalOptions_bSelectedRow = 0;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 4, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        object->flags |= 0x28;
        gPalOptions_pLabelSprite = (DisplayObjectConfig *)object;
        gPalOptions_bSoundOutputType = gSD_bOutputType;
        if (gPalOptions_bSoundOutputType < 0) {
            gPalOptions_bSoundOutputType = 0;
        }

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, gPalOptions_bSoundOutputType, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        gPalOptions_pSoundTypeSprite = (DisplayObjectConfig *)object;
        object->flags |= 0x28;
        gPalOptions_bState = 2;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 20, 60, 3, 0, 2, 0xB, 0x20C);
        gPalOptions_pCursor = object;
        object->flags |= 0x28;
        PalOptions_PositionCursor(gPalOptions_bSelectedRow);
        gPalOptions_dwWavePhase = 0;
        music = 0x7350;
    }
    SD_BGMPlay(music);
}
