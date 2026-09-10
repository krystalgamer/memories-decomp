#include "../types.h"
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "main_mode_state.h"
#include "duel_effect_mode_7.h"

extern u8 D_8009B2F8;

void func_80033C90(void)
{
    D_8009B2F8 = 0;
    D_8009B26C[0] = 7;
}

void func_80033CA8(void)
{
    D_8009B2F8 = 0x80;
    D_8009B26C[0] = 7;
}
