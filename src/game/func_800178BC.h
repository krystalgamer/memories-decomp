#ifndef MEMORIES_DECOMP_FUNC_800178BC_H
#define MEMORIES_DECOMP_FUNC_800178BC_H

#include "../types.h"

/* Re-establishes the geometry state after func_80017130: sets the projection
 * distance and the 0xA0/0x6C screen offset, writes the two constants and the
 * angle into D_800F2848, refreshes the light matrix through GsSetLsMatrix,
 * and then projects the fixed scratchpad vector through official Psy-Q GTE
 * inline macros.
 *
 * D_8009B200 and D_8009B202 receive the projected X/Y offsets from the
 * 0xA0/0x6C screen centre.
 *
 * Duel_InitScene (src/candidates/duel_init_scene.c) is the only consumer and
 * reached it through a local extern that already agreed with this. */
void func_800178BC(void);
extern s16 D_8009B200;
extern s16 D_8009B202;

#endif
