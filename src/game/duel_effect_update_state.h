#ifndef MEMORIES_DECOMP_DUEL_EFFECT_UPDATE_STATE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_UPDATE_STATE_H

#include "../types.h"

/* Advances the duel-effect state machine by one step and reports whether it
   is still running: 0 once D_8009B254 is clear or its 0x40 bit is set, 1
   otherwise. On the first step of a new state it latches the state index into
   D_8009B24A; afterwards it runs that state's handler from
   gDuelEffect_apfnStateHandler. */
s32 DuelEffect_UpdateState(void);

#endif
