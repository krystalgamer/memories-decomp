#ifndef MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H
#define MEMORIES_DECOMP_TWO_PLAYER_SAVE_SETUP_H

#include "../types.h"

/* The two-player save unit. Both functions were reached through local externs
 * before this header existed, and every one of those declarations already
 * agreed with its definition.
 *
 * func_8003FD14 reports a status its callers poll: frontend.c stores the
 * result and async_state_poll.c tests it.
 *
 * func_8003FE14 stamps integrity over both slots of the two-player save at
 * D_801D1880, records the second slot in D_8009B3E0, and hands the pair to
 * the transfer. trade_update.c calls it once.
 *
 * Both keep their address-based names: functions.csv carries no semantic name
 * for either address, and what the polled status means is decided by the
 * callers rather than here. */
s32 func_8003FD14(void);
void func_8003FE14(void);

#endif
