#ifndef MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H
#define MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H

#include "../types.h"

/* The mode byte for the "pick a card off the field" cursor, at 0x2CC($gp).
 * Duel_UpdateCardPickCursor owns it, and its own comment gives the bits:
 *
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until D_8009B162 expires
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 *
 * Zero therefore means the cursor is not active at all, which is how
 * func_800179F4 uses it: it clears this byte in the run of assignments that
 * resets the duel, alongside D_8009B162, the countdown the 0x40 hold waits
 * on.
 *
 * func_8001BD88 is the third accessor and is still assembly; it reads and
 * writes the byte gp-relative like everything else here. */
extern u8 D_8009B1D4;

#endif
