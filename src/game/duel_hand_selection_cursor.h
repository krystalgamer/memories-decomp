#ifndef MEMORIES_DECOMP_DUEL_HAND_SELECTION_CURSOR_H
#define MEMORIES_DECOMP_DUEL_HAND_SELECTION_CURSOR_H

#include "duel_selection_layout.h"

/* Creates the current hand slot's order-marker sprite, lifts the card and
 * records the incremented count in selection_order. The depth calculation reads
 * depth_offset as u8 before adding one and narrowing to s8. */
void DuelHand_CreateSelectionOrderMarker(DuelSelectionRecord *state);

#endif
