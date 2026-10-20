#ifndef MEMORIES_DECOMP_DUEL_HAND_SELECTION_COLORS_H
#define MEMORIES_DECOMP_DUEL_HAND_SELECTION_COLORS_H

#include "../types.h"
#include "duel_selection_layout.h"

/* Dims only unmarked slots. With no ordered picks, restores the cursor's
 * card; selected cards otherwise retain their existing colors. */
void DuelHand_RefreshSelectionColors(DuelSelectionRecord *side);

#endif
