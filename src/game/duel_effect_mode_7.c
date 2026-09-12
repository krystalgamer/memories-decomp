#include "../types.h"
#include "duel_effect_mode_7.h"
#include "build_deck_transition_state.h"

extern u8 D_8009B2F8;
extern u8 D_8009B26C[];

void func_80033C90(void)
{
    D_8009B2F8 = 0;
    D_8009B26C[0] = 7;
}

void func_80033CA8(void)
{
    D_8009B2F8 = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    D_8009B26C[0] = 7;
}
