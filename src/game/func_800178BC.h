#ifndef MEMORIES_DECOMP_FUNC_800178BC_H
#define MEMORIES_DECOMP_FUNC_800178BC_H

#include "../types.h"

/* Re-establishes the geometry state after func_80017130: sets the projection
 * distance and the 0xA0/0x6C screen offset, writes the two constants and the
 * angle into D_800F2848, refreshes the light matrix through GsSetLsMatrix,
 * and then runs a hand-written GTE block that drives the coprocessor
 * directly.
 *
 * The inline assembly is the reason this unit stays as it is: the block
 * writes the GTE control registers and issues rtps by encoded word, so the
 * sequence is pinned rather than expressed in C. It is also why the
 * definition is a candidate, src/candidates/func_800178BC.c.
 *
 * func_800179F4 (src/candidates/func_800179F4.c) is the only consumer and
 * reached it through a local extern that already agreed with this. */
void func_800178BC(void);

#endif
