#ifndef MEMORIES_DECOMP_FUNC_8002F9D4_H
#define MEMORIES_DECOMP_FUNC_8002F9D4_H

#include "../types.h"

/* D_80090C50 handler: the conditional script jump on deck state. Always
 * consumes a 16-bit offset, and repoints D_8009B290 into D_801A8000 only when
 * func_8002EE5C reports the deck is NOT full -- so the jump is the
 * deck-incomplete path and falling through is the complete one. */
void func_8002F9D4(void);

#endif
