#ifndef MEMORIES_DECOMP_BUILD_DECK_DECK_CAPACITY_H
#define MEMORIES_DECOMP_BUILD_DECK_DECK_CAPACITY_H

#include "../types.h"

/* Reports whether the deck has a free slot.
 *
 * It walks DECK_SIZE entries of sixteen bytes each, starting 0x2D50 into the
 * Build Deck workspace, and returns 1 as soon as it finds one whose
 * byte at +0x0D is zero. It returns 0 when every entry is occupied.
 *
 * func_800339D0.c is the only consumer, and its local extern already agreed
 * apart from spelling the return s32 where the definition writes int. */
int BuildDeck_HasOpenDeckSlot(void);

#endif
