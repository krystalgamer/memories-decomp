#include "../types.h"
#include "duel_effect_mode_7.h"
#include "main_mode_state.h"
#include "build_deck_transition_state.h"
#include "../unmatched.h"
#include "build_deck_draw_card_list_page_data.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gBuildDeck_bConfirmMode;

void BuildDeck_EnterNarrowConfirmMode(void)
{
    gBuildDeck_bConfirmMode = 0;
    gMain_bActiveModeState = 7;
}

void BuildDeck_EnterWideConfirmMode(void)
{
    gBuildDeck_bConfirmMode = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    gMain_bActiveModeState = 7;
}
