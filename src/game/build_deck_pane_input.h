#ifndef MEMORIES_DECOMP_BUILD_DECK_PANE_INPUT_H
#define MEMORIES_DECOMP_BUILD_DECK_PANE_INPUT_H

#include "../types.h"

/* The two input steps of the Build Deck step table D_80090DF8
 * (duel_transition_step_table.c). Both take the BuildDeckTransitionState
 * as the table's u8 * and cast it on entry.
 *
 * func_800336F0 is step 2, the chest pane. Its confirm moves the highlighted
 * card into the deck: BuildDeck_AddCard adds it and func_80031F7C takes one
 * off the chest count. RIGHT slides the viewport to 0x140 through
 * pane-transition step 1, landing in step 3.
 *
 * func_8003353C is step 3, the deck pane, and does the reverse. Its confirm
 * clears the highlighted deck record, recounts the deck (func_8003201C) and
 * puts the card back in the chest (func_80031EE4). LEFT slides back to 0 and
 * returns to step 2.
 *
 * In both, TRIANGLE opens the card viewer on the highlighted card, and
 * CANCEL enters exit step 4 (func_800339D0) with next_state recording which
 * of the two to come back to. */
void func_8003353C(u8 *p);
void func_800336F0(u8 *p);

#endif
