#ifndef MEMORIES_DECOMP_BUILD_DECK_UPDATE_H
#define MEMORIES_DECOMP_BUILD_DECK_UPDATE_H

#include "build_deck_transition_state.h"

/* Exit step 4 of the Build Deck step table D_80090DF8
 * (duel_transition_step_table.c). It asks for confirmation and either
 * returns to the step saved in next_state or writes the deck out and clears
 * the state word. */
void BuildDeck_UpdateExitConfirmation(BuildDeckTransitionState *record);

/* The per-frame Build Deck driver. It pulses the two pane colours with a
 * triangle wave, dispatches D_80090DF8[gBuildDeck_pState->state & 0x3F] once
 * DuelEffect_UpdateState reports idle, and returns the state word. The exit
 * step above clears that word when it commits the deck, and both callers
 * (Main_RunBuildDeckMenu and Main_RunDuel) leave the menu when it reads zero.
 */
s32 BuildDeck_Update(void);

#endif
