#ifndef MEMORIES_DECOMP_DUEL_REWARD_SETUP_H
#define MEMORIES_DECOMP_DUEL_REWARD_SETUP_H

#include "../types.h"

/* Queues the reward table transfer: one async read of 0x4C sectors from
 * 0x2189 with func_80032184 as the completion callback, then waits for it.
 * main_run_trade.c is the only consumer. */
void func_80032328(void);

/* Compacts gDuel_awRecentCardDrops in two passes. The first walks the table
 * backwards and clears any entry whose referenced record byte, read from the
 * region 0x56C below the table, is zero. The second moves the surviving
 * entries to the front, zeroing each slot it moves out of, so the table ends
 * up densely packed with the order preserved.
 *
 * The unit reaches the table through an inline block that materialises its
 * address into a pinned register rather than by naming it directly, and both
 * loops run on pinned registers. That spelling is load bearing, so the
 * definition is not to be tidied into ordinary C without a measurement.
 *
 * func_800339D0.c is the only consumer. Both names stay address-based: the
 * two passes are legible, but what makes a drop recent, and what the record
 * byte below the table means, are not settled here. */
void func_80032370(void);

#endif
