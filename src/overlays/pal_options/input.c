#include "../../types.h"
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#include "../../game/input.h"
#include "../../game/sound.h"
#include "helpers.h"

void PalOptions_HandleInput(void)
{
    s32 selection = gPalOptions_bSelectedRow;

    if (selection == 0) {
        if ((gInput_wPad1Pressed & PAD_DIRECTION_RIGHT) != 0 && gPalOptions_bSoundOutputType == 0) {
            gSD_bOutputType = 1;
            gPalOptions_bSoundOutputType = 1;
            SD_SetOutputType(1);
            SD_SEPlayFull(0x2F);
        }
        if ((gInput_wPad1Pressed & PAD_DIRECTION_LEFT) != 0 && gPalOptions_bSoundOutputType == 1) {
            gSD_bOutputType = 0;
            gPalOptions_bSoundOutputType = 0;
            SD_SetOutputType(0);
            SD_SEPlayFull(0x2F);
        }
    } else {
        if ((gInput_wPad1Repeat & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            selection = gPalOptions_bLanguage;
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                selection++;
                if (selection >= 5) {
                    selection = 0;
                }
            } else {
                selection--;
                if (selection < 0) {
                    selection = 4;
                }
            }
            gPalOptions_bLanguage = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pLanguageSprite, selection);
            gPalOptions_pLabelSprite->resource_branch_index = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pLabelSprite, gPalOptions_pLabelSprite->resource_variant);
            gPalOptions_pSoundTypeSprite->resource_branch_index = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pSoundTypeSprite, gPalOptions_pSoundTypeSprite->resource_variant);
            SD_SEPlayFull(0x2F);
        }
    }
    if ((gInput_wPad1Pressed & (PAD_BUTTON_CIRCLE | PAD_BUTTON_CROSS)) != 0) {
        gPalOptions_bState = 3;
        SD_SEPlayFull(8);
    }
}
