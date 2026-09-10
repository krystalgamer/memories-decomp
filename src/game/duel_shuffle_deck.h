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
 * Duel_ShuffleDeck's own prototype is here now. It was left out while
 * duel_shuffle_both_decks.c declared `(void *, void *, void *)` against this
 * definition's `(s32 src, u8 *out16, u8 *out8)`, on the grounds that whether
 * the pointer-versus-s32 first parameter was load bearing at that call had
 * not been measured.
 *
 * It is not. The shared prototype takes the definition's spelling and the
 * caller converts at the call site, and the executable is unchanged. `src`
 * is an address the body walks with `src += 2` and reads through
 * `*(u16 *)src`, so s32 describes what the parameter actually holds, and
 * the one caller that passes a pointer says so with a cast.
 */
extern u8 gDuel_awOpponentDeckPool[];

void Duel_ShuffleDeck(s32 src, u8 *out16, u8 *out8);

#endif
