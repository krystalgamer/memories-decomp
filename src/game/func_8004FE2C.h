#ifndef MEMORIES_DECOMP_FUNC_8004FE2C_H
#define MEMORIES_DECOMP_FUNC_8004FE2C_H

#include "../types.h"

/* Called from model_scene_states.c. The signed phase byte D_8009AF9A selects
 * a step through a 17-entry jump table over (s8)(D_8009AF9A + 1); most steps
 * compare slot zero's field_E06 against a threshold, call func_8005F3B8,
 * func_8005D994 or func_800597C8, and advance the phase. */
void func_8004FE2C(void);

#endif
