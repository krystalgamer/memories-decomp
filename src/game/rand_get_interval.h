#ifndef MEMORIES_DECOMP_RAND_GET_INTERVAL_H
#define MEMORIES_DECOMP_RAND_GET_INTERVAL_H

#include "../types.h"

/* rand() reduced modulo `divisor`, so the result is 0..divisor-1. Callers pass
   a count, not a maximum. */
s32 Rand_GetInterval(s32 divisor);

#endif
