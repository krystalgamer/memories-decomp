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

/* The base those four lists are reached from: the player's shuffled deck.
 * Duel_ShuffleBothDecks is the only source in the tree that names it, and it
 * declared the symbol for itself because this header did not.
 *
 * The offsets above are not free-standing numbers -- config/slus_01411/symbols.txt
 * names what each one lands on. From this symbol at 0x80177FE8,
 * `+ DECK_SIZE * sizeof(u16)` is gDuel_awOpponentShuffledDeck (0x80178038),
 * `- 0x54` is gDuel_awPlayerDeckShuffle (0x80177F94) and `- 0x2C` is
 * gDuel_awOpponentDeckShuffle (0x80177FBC). The distance to the next named
 * symbol is 0x50, which is DECK_SIZE halfwords exactly, so the player's list
 * ends where the opponent's begins.
 *
 * That distance is recorded as evidence about the extent, not asserted as a
 * bound: the declaration stays the incomplete array the one declarer wrote,
 * and stays char, with the u8 * conversions left at the call sites where that
 * declarer put them. It is the arrangement duel_shuffle_deck.h already records
 * for gDuel_awOpponentDeckPool, whose halfword reads are spelled as a cast at
 * the use. */
extern char gDuel_awPlayerShuffledDeck[];

#endif
