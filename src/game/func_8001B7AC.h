#ifndef MEMORIES_DECOMP_FUNC_8001B7AC_H
#define MEMORIES_DECOMP_FUNC_8001B7AC_H

#include "duel_hand.h"

/* Creates the current hand slot's order-marker sprite, lifts the card and
 * records the incremented count in active_09. The depth calculation reads
 * field_16 as u8 before adding one and narrowing to s8. */
void func_8001B7AC(DuelHandStackState *state);

#endif
