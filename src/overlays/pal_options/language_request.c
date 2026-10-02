#include "../../types.h"
#include "helpers.h"

void func_80168D34(s32 language)
{
    if (D_8009C02B != language) {
        D_8009C02B = language;
        func_80043BC8(D_8009C02B, 0);
    }
}
