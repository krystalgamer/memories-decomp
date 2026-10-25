#include "../../types.h"
#include "helpers.h"

void PalOptions_RequestLanguage(s32 language)
{
    if (gText_bLanguageIndex != language) {
        gText_bLanguageIndex = language;
        Main_RequestLanguageBootPackage(gText_bLanguageIndex, 0);
    }
}
