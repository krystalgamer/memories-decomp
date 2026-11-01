#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "duel_effect_mode_7.h"
#include "main_mode_state.h"
#include "build_deck_transition_state.h"
#include "../unmatched.h"
#include "build_deck_draw_card_list_page_data.h"

void BuildDeck_EnterNarrowConfirmMode(void)
{
    gBuildDeck_bConfirmMode = 0;
    gMain_bActiveModeState[0] = 7;
}

void BuildDeck_EnterWideConfirmMode(void)
{
    gBuildDeck_bConfirmMode = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    gMain_bActiveModeState[0] = 7;
}
