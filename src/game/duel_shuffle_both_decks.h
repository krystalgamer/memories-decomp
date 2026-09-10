#ifndef MEMORIES_DECOMP_DUEL_SHUFFLE_BOTH_DECKS_H
#define MEMORIES_DECOMP_DUEL_SHUFFLE_BOTH_DECKS_H

#include "../types.h"

/* Shuffles both duelists' decks into gDuel_awPlayerShuffledDeck, running
 * Duel_ShuffleDeck once per side. Each side gets a halfword id list and a
 * byte order list, and the byte lists sit below the halfword base at -0x54
 * and -0x2C.
 *
 * func_800179F4 declared this `(u8 *, u8 *)` while the definition takes
 * `(void *, void *)`; nothing compared the two. */
void Duel_ShuffleBothDecks(void *arg0, void *arg1);

#endif
