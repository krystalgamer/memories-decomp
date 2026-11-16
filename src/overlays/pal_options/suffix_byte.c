#include "../../types.h"
#include "helpers.h"

/* Unoptimized duplicate of PalOptions_GetLanguage (options_screen.c) over
   its own suffix byte. No caller or reference to it is known. */
s32 PalOptions_GetLanguageCopy(void)
{
    return gPalOptions_bLanguageCopy;
}
