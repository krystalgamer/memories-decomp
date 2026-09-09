#ifndef MEMORIES_DECOMP_FUNC_8002F440_H
#define MEMORIES_DECOMP_FUNC_8002F440_H

#include "../types.h"

/* D_80090C50 handler: starts a fade out. The low six bits of its stream byte,
 * when set, become the fade step in D_800E9ECF; bit 7 keeps the command busy
 * until D_800E9ECE's own bit 7 clears, which is how the script waits for the
 * fade rather than only starting it. */
void func_8002F440(void);

#endif
