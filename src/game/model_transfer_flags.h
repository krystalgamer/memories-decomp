#ifndef MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H
#define MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H

#include "../types.h"

/* Two independent flag bytes, D_8009B07B and D_8009B07C, each with a getter
 * and a setter.
 *
 * The accessors are not the only route to them: model_effect_state.c and
 * three func_ files read the two bytes directly rather than through the
 * getters, which is why the bytes are declared here as well.
 *
 * Six callers share one guard, spelled as
 *
 *     state = func_8005F174();
 *     if (state != 1 || func_8005F18C() != state) { ... }
 *
 * so the interesting condition is that both flags read exactly 1. The setter
 * for the first is called with values other than 0 and 1, which is why the
 * callers compare against 1 rather than testing for non-zero.
 */
extern u8 D_8009B07B;
extern u8 D_8009B07C;

s32 func_8005F174(void);
void func_8005F180(s32 value);
s32 func_8005F18C(void);
void func_8005F198(s32 value);

#endif
