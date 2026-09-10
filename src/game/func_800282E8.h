#ifndef MEMORIES_DECOMP_FUNC_800282E8_H
#define MEMORIES_DECOMP_FUNC_800282E8_H

#include "../types.h"

/* A once-only latch over bit 0x80 of D_8009B248. It returns 0 the first time
 * it is asked, setting the bit as it goes, and 1 on every call after that.
 * Both consumers use it the same way, as the guard on a first-entry path.
 *
 * The address-based name is kept: what the latched path sets up is described
 * by the callers rather than by this function, so naming it would be a guess. */
int func_800282E8(void);

#endif
