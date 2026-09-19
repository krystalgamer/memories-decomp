#ifndef MEMORIES_DECOMP_MODEL_EFFECT_STATE_H
#define MEMORIES_DECOMP_MODEL_EFFECT_STATE_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../ygo_types.h"

/* Model-effect setup, state, endpoint queueing and dispatch.
 *
 * func_8005F3B8's last parameter is a pointer in the definition -- the SVECTOR
 * whose offset the step applies -- not the fifth s32 its two callers declared.
 * model_scene_states.c passes literal zeroes, which were always null pointers;
 * func_8005F27C and func_8005F5C8 forward the same optional pointer type.
 *
 * func_8005F714 takes two indices into gModel_aEffectEndpoints rather than
 * pointers, and a negative index means "no endpoint" -- that is how a caller
 * asks for a one-sided effect, and func_800556E8.c passes -1 for exactly that.
 *
 * func_8005F91C fills the keyframe ring that func_8005EBF4 evaluates. Its two
 * endpoint pointers are the narrowest common view used by its callers, which
 * pass ModelEffectEndpoint pointers, a local s16[4], or generic byte pointers.
 * func_8005F828 therefore retains void * parameters, but its post-step stop
 * tests use the typed kind member through an aligned s16 view; retail loads
 * that field with lh, while direct access through the packed type expands it.
 * These operations return early while D_8009B07B and D_8009B07C are both 1. */
void func_8005F27C(s32 mode, s32 coefficient, SVECTOR *offset);
void func_8005DBA4(void);
void func_8005F3B8(s32 mode, s32 y, s32 a, s32 b, SVECTOR *offset);
s32 func_8005F564(void);
void func_8005F588(s32 value);
void func_8005F5C8(s32 mode, s32 coefficient, SVECTOR *offset, s32 arg);
void func_8005F714(s32 first, s32 second, s32 arg);
void func_8005F7B0(s32 value, s32 arg);
void func_8005F828(s32 count, void *first, void *second, s32 arg);
void func_8005F91C(
    s32 mode,
    ModelEffectEndpoint *first,
    ModelEffectEndpoint *second,
    s32 arg
);

#endif
