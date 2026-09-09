#ifndef MEMORIES_DECOMP_FUNC_8005F91C_H
#define MEMORIES_DECOMP_FUNC_8005F91C_H

#include "../types.h"

/* Fills one entry of the keyframe ring that func_8005EBF4 evaluates: `a` and
 * `b` are the two endpoint records, either of which may be null, and `mode`
 * says whether this entry starts a run or continues one. It returns without
 * touching anything while the two D_8009B07B/D_8009B07C gates are both 1.
 *
 * The endpoints are u8 * in the definition, which is the narrowest of the three
 * views in play: func_8005D994.c already casts to it, while model_effect_state.c
 * reaches it with ModelEffectEndpoint *, a local s16[4] and a pair of void *,
 * and casts at each of its three call sites. Same memory in every case. */
void func_8005F91C(s32 mode, u8 *a, u8 *b, s32 arg);

#endif
