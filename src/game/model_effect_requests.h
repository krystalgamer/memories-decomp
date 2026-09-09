#ifndef MEMORIES_DECOMP_MODEL_EFFECT_REQUESTS_H
#define MEMORIES_DECOMP_MODEL_EFFECT_REQUESTS_H

#include "../types.h"

/* Two guarded requests into func_80052D2C.
 *
 * Both consult func_8005F174 and func_8005F18C first and do nothing when the
 * state is 1 and the two agree, so a caller cannot assume the request was
 * issued. Neither reports whether it was.
 *
 * func_80059F18 biases its middle two arguments before passing them on: a
 * negative becomes 0 and anything else is incremented, so the values reaching
 * func_80052D2C are not the ones the caller wrote. */
void func_80059EBC(s32 value);
void func_80059F18(s32 first, s32 second, s32 third, s32 fourth);

#endif
