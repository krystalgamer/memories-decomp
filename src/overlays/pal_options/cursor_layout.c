#include "../../types.h"
#include "helpers.h"

void PalOptions_PositionCursor(s32 selection)
{
    gPalOptions_pCursor->field_30.h.field_30 = gPalOptions_bSoundOutputType * 112 + 60;
    gPalOptions_pCursor->field_30.h.field_32 = 80;
    if (selection != 0) {
        gPalOptions_pCursor->field_30.h.field_32 = 112;
    }
    DisplayObject_UpdateResourceVariant(gPalOptions_pSoundTypeSprite, gPalOptions_bSoundOutputType);
}
