#include "../types.h"
#include "func_800282E8.h"
#include "duel_effect.h"

int func_800282E8(void)
{
    unsigned char value = D_8009B248;

    if (!(value & DUEL_EFFECT_DIALOG_FLAG_CREATED)) {
        D_8009B248 = value | DUEL_EFFECT_DIALOG_FLAG_CREATED;
        return 0;
    }
    return 1;
}
