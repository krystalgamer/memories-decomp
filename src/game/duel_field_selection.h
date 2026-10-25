#ifndef MEMORIES_DECOMP_DUEL_FIELD_SELECTION_H
#define MEMORIES_DECOMP_DUEL_FIELD_SELECTION_H

#include "../types.h"
#include "duel_selection_layout.h"

/* Initializes the active side's placement cursor, restricts its row, chooses
 * a free column when available and requests the selection display. */
void Duel_BeginFieldPlacementSelection(DuelSelectionRecord *selection);

/* Applies the script's card picks to the hand through record swaps, display
 * replacement and conversion to hand-slot selection codes. */
void Duel_ApplyAiHandSelection(void);

#endif
