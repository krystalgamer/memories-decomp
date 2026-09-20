#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "duel_effect_mode_7.h"
#include "main_mode_state.h"
#include "build_deck_transition_state.h"
#include "../unmatched.h"

void func_80033C90(void)
{
    D_8009B2F8 = 0;
    D_8009B26C[0] = 7;
}

void BuildDeck_EnterWideConfirmMode(void)
{
    D_8009B2F8 = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    D_8009B26C[0] = 7;
}
