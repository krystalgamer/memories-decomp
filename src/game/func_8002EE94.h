#ifndef MEMORIES_DECOMP_FUNC_8002EE94_H
#define MEMORIES_DECOMP_FUNC_8002EE94_H

#include "../types.h"

/* D_80090C50 handler: the dialog choice step, and the deck test it shares with
 * func_8002F9D4.
 *
 * func_8002EE5C reports whether the player's deck is full -- 1 when no
 * DECK_SIZE entry of gDuel_awPlayerDeck is zero, 0 as soon as one is -- and
 * func_8002F9D4 branches the script on it. It lives here because this is the
 * unit that defines it; nothing about it is specific to the choice step. */
void func_8002EE94(void);
s32 func_8002EE5C(void);

#endif
