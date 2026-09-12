#ifndef MEMORIES_DECOMP_DUEL_REWARD_SETUP_H
#define MEMORIES_DECOMP_DUEL_REWARD_SETUP_H

#include "../types.h"
#include "file_transfer.h"

/* Four-phase callback for the 0x2189, 0x4C-sector duel reward request. */
void func_80032184(FileTransferDescriptor *descriptor, s32 mode);

extern s16 gDuel_awRecentCardDrops[];

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
 * The split compiler profile naturally keeps the table address and loop
 * cursors in the retail registers when the table is named directly.
 *
 * func_800339D0.c is the only consumer. Both names stay address-based: the
 * two passes are legible, but what makes a drop recent, and what the record
 * byte below the table means, are not settled here. */
void func_80032370(void);

#endif
