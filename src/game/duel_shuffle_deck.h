#ifndef MEMORIES_DECOMP_DUEL_SHUFFLE_DECK_H
#define MEMORIES_DECOMP_DUEL_SHUFFLE_DECK_H

#include "../types.h"

/* The opponent's drop-weight table, loaded per opponent and drawn from.
 *
 * func_800179F4 fills it, handing the address to a transfer as the
 * destination and choosing the source from the opponent id:
 *
 *     File_RequestAsyncTransfer(0, 0, gDuel_bOpponentID * 2 + gDuel_bOpponentID
 *                               + 7475, 3, 0, 0, (s32)gDuel_awOpponentDeckPool);
 *
 * so three sectors per opponent. Duel_ShuffleDeck then reads it as halfword
 * weights: it draws a limit in 1..DUEL_DROP_WEIGHT_TOTAL and walks the table
 * accumulating `*(u16 *)e` until the running total reaches it, which is a
 * weighted pick rather than a list of card ids.
 *
 * Declared u8 with the halfword read spelled as a cast at the use, which is
 * what both sources already said.
 *
 * Duel_ShuffleDeck itself is deliberately absent. Its definition is
 * `(s32 src, u8 *out16, u8 *out8)` and duel_shuffle_both_decks.c declares
 * `(void *, void *, void *)`, passing its own `void *` parameters straight
 * through; the body then tests `src == 0`. Whether the pointer-versus-s32
 * first parameter is load bearing at that call has not been measured, and a
 * shared prototype would have to pick one of the two. It needs its own
 * change.
 */
extern u8 gDuel_awOpponentDeckPool[];

#endif
