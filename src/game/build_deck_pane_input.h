#ifndef MEMORIES_DECOMP_BUILD_DECK_PANE_INPUT_H
#define MEMORIES_DECOMP_BUILD_DECK_PANE_INPUT_H

#include "build_deck_transition_state.h"
#include "card_list_text_boxes.h"

/* Returns the id of the entry the cursor is on, or 0 when that row's flag
   byte is clear. Both pane-input steps read the highlighted card through it. */
s32 BuildDeck_GetActiveCardID(CardList *list);

/* The two input steps of the Build Deck step table gBuildDeck_apfnStepHandlers
 * (duel_transition_step_table.c). Both receive the shared transition state;
 * their bodies retain a byte-pointer alias for offset-heavy workspace access.
 *
 * BuildDeck_UpdateChestPaneInput is step 2, the chest pane. Its confirm moves the highlighted
 * card into the deck: BuildDeck_AddCard adds it and BuildDeck_TakeCardFromChest takes one
 * off the chest count. RIGHT slides the viewport to 0x140 through
 * pane-transition step 1, landing in step 3.
 *
 * BuildDeck_UpdateDeckPaneInput is step 3, the deck pane, and does the reverse. Its confirm
 * clears the highlighted deck record, recounts the deck (BuildDeck_RecountDeckCards) and
 * puts the card back in the chest (BuildDeck_ReturnCardToChest). LEFT slides back to 0 and
 * returns to step 2.
 *
 * In both, TRIANGLE opens the card viewer on the highlighted card, and
 * CANCEL enters exit step 4 (BuildDeck_UpdateExitConfirmation) with next_state recording which
 * of the two to come back to. */
void BuildDeck_UpdateDeckPaneInput(BuildDeckTransitionState *state);
void BuildDeck_UpdateChestPaneInput(BuildDeckTransitionState *state);

#endif
