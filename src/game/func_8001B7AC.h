#ifndef MEMORIES_DECOMP_FUNC_8001B7AC_H
#define MEMORIES_DECOMP_FUNC_8001B7AC_H

#include "duel_hand.h"

/* Spawns the next card in a hand-stack animation. The source card remains a
 * byte pointer in DuelHandSlot, but its depth byte is indexed through the
 * asserted DisplayObject::field_16 offset. */
void func_8001B7AC(DuelHandStackState *state);

#endif
