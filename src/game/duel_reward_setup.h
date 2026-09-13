#ifndef MEMORIES_DECOMP_DUEL_REWARD_SETUP_H
#define MEMORIES_DECOMP_DUEL_REWARD_SETUP_H

#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"

/* 0x801D07BC, 0x56C bytes above gLibrary_abCardChest (0x801D0250). */
extern s16 gDuel_awRecentCardDrops[DUEL_RECENT_CARD_DROP_COUNT];
#define DUEL_RECENT_DROPS_CHEST_DISTANCE 0x56C

/* Four-phase callback for the 0x2189, 0x4C-sector duel reward request. */
void func_80032184(FileTransferDescriptor *descriptor, s32 mode);

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
 * The ordinary C uses the split-address compiler profile for the initial
 * table address. Its reverse cursor is an integer address, so the final
 * decrement does not form a pointer before the array.
 *
 * func_800339D0.c is the only consumer. Both names stay address-based: the
 * two passes are legible, but what makes a drop recent, and what the record
 * byte below the table means, are not settled here. */
void func_80032370(void);

#endif
