#include "../../types.h"
#include "helpers.h"

void PalOptions_RequestLanguage(s32 language)
{
    if (gText_bLanguageIndex != language) {
        gText_bLanguageIndex = language;
        func_80043BC8(gText_bLanguageIndex, 0);
    }
}
