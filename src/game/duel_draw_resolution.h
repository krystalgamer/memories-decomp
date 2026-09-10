#ifndef MEMORIES_DECOMP_DUEL_DRAW_RESOLUTION_H
#define MEMORIES_DECOMP_DUEL_DRAW_RESOLUTION_H

#include "../types.h"

/* Returns nonzero when all five Exodia pieces are in hand. It consumes a local
 * copy of the hand and blanks each match, so a single hand entry cannot
 * satisfy two pieces. */
s32 Duel_HasAllExodiaPieces(void);

void func_80018DB4(void);

#endif
