#ifndef MEMORIES_DECOMP_DUEL_SHUFFLE_DECK_H
#define MEMORIES_DECOMP_DUEL_SHUFFLE_DECK_H

#include "../types.h"

/* The opponent's drop-weight table, loaded per opponent and drawn from.
 *
 * Duel_InitScene fills it, handing the address to a transfer as the
 * destination and choosing the source from the opponent id:
 *
 *     File_RequestAsyncTransfer(0, 0, gDuel_bOpponentID * 2 + gDuel_bOpponentID
 *                               + 7475, 3, 0, 0, (s32)gDuel_awOpponentDeckPool);
 *
 * so three sectors per opponent. Duel_ShuffleDeck then reads it as halfword
 * weights: it draws a limit in 1..DUEL_DROP_WEIGHT_TOTAL and walks the table
 * accumulating unsigned halfwords until the running total reaches it, which is a
 * weighted pick rather than a list of card ids.
 *
 * The incomplete u16 array models those weights without assigning the whole
 * three-sector transfer a new fixed table extent.
 *
 * The shuffle source is an optional read-only halfword list: null selects
 * weighted generation instead. The two outputs are DECK_SIZE halfword card
 * IDs and DECK_SIZE original-slot bytes, permuted together. Pointer steps
 * advance one element, preserving the measured two-byte and one-byte walks.
 * No card-ID validation, source/output separation or new bound is imposed.
 */
extern u16 gDuel_awOpponentDeckPool[];

void Duel_ShuffleDeck(const u16 *src, u16 *out16, u8 *out8);

#endif
