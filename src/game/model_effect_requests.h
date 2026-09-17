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

/* Points the camera move's target at slot `second` of pair `first`, under the
 * same func_8005F174/func_8005F18C guard: nothing is written when the state
 * is 1 and the two agree. */
void func_8005A010(s32 first, s32 second);

/* Stores twice `value` into the camera move's D_800F2B22, under that same
 * guard -- written with the test inverted, so the store sits inside
 * `state != 1 || func_8005F18C() != state` rather than after an early return
 * (model_effect_requests.c:73).
 *
 * This unit defines seven functions and this header declares the ones with
 * consumers outside it. func_8005A0DC was the fourth such case and the only
 * one whose caller wrote the prototype by hand: func_8004EB00, the model scene
 * mode 15 controller, calls it once with 0x78
 * (src/candidates/func_8004EB00.c:326) while already including this header.
 * Its two immediate neighbours in the file, func_8005A074 and func_8005A130,
 * have no uses outside the unit at all, which is why they are still not
 * declared here. */
void func_8005A0DC(s32 value);

#endif
