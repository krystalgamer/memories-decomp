#include "../../types.h"
#include "../../game/fade.h"
#include "../../game/file_transfer.h"
#include "../../game/sound.h"
#include "helpers.h"

s32 Options_Update(void)
{
    switch (gPalOptions_bState & 0xF) {
    case 1:
        PalOptions_HandleInput();
        break;
    case 2:
        if ((gPalOptions_bState & 0x80) == 0) {
            gPalOptions_bState |= 0x80;
            Fade_StartIn();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gPalOptions_bState = 1;
        }
        break;
    case 3:
        if ((gPalOptions_bState & 0x80) == 0) {
            gPalOptions_bState |= 0x80;
            PalOptions_RequestLanguage(gPalOptions_bLanguage);
            SD_BGMFadeOut();
            Fade_StartOut();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0 &&
            ((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134) == 0) {
            PalOptions_StoreLanguageImage();
            gPalOptions_bState = 0;
        }
        break;
    }
    PalOptions_PositionCursor(gPalOptions_bSelectedRow);
    if (gPalOptions_bState != 0) {
        return -1;
    }
    return gPalOptions_bLanguage;
}
