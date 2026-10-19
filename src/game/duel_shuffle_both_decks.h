#ifndef MEMORIES_DECOMP_DUEL_SHUFFLE_BOTH_DECKS_H
#define MEMORIES_DECOMP_DUEL_SHUFFLE_BOTH_DECKS_H

#include "../types.h"

/* Shuffles both duelists' decks into gDuel_awPlayerShuffledDeck, running
 * Duel_ShuffleDeck once per side. Each side gets a halfword id list and a
 * byte order list, and the byte lists sit below the halfword base at -0x54
 * and -0x2C.
 *
 * Each source is an optional read-only halfword card-ID list. Null retains
 * Duel_ShuffleDeck's weighted-generation path. */
void Duel_ShuffleBothDecks(const u16 *arg0, const u16 *arg1);

/* The halfword base those four lists are reached from: the player's shuffled
 * card IDs. Duel_ShuffleBothDecks is the only C source that names this base.
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
 * bound: the declaration stays incomplete. Its u16 elements match the two-byte
 * ID outputs. The caller retains one base and uses byte views only for the
 * permutation-list addresses; all four offsets are unchanged. This does not
 * combine the lists or retype the adjacent byte output buffers. */
extern u16 gDuel_awPlayerShuffledDeck[];

#endif
