#ifndef MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H
#define MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H

#include "../types.h"

/* Two updates to the build-deck screen's per-card counts, both taking the
 * screen state block rather than an index into it.
 *
 * func_80031F7C decrements one card's count and, when it reaches zero, clears
 * that card's deck record flag and re-sorts. It does nothing at all when the
 * count is already zero, and returns nothing to say so.
 *
 * func_8003201C recounts every card from the deck records and stores how many
 * leading records are still marked. */
void func_80031F7C(u8 *state, s32 id);
void func_8003201C(u8 *state);

#endif
