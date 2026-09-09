#ifndef MEMORIES_DECOMP_MODEL_EFFECT_STATE_H
#define MEMORIES_DECOMP_MODEL_EFFECT_STATE_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Two of the effect-state steps other units reach.
 *
 * func_8005F3B8's last parameter is a pointer in the definition -- the SVECTOR
 * whose offset the step applies -- not the fifth s32 its two callers declared.
 * model_scene_states.c passes a literal 0, which was always a null pointer;
 * func_8005F27C.c forwards its own s32 argument and casts.
 *
 * func_8005F714 takes two indices into gModel_aEffectEndpoints rather than
 * pointers, and a negative index means "no endpoint" -- that is how a caller
 * asks for a one-sided effect, and func_800556E8.c passes -1 for exactly that.
 * Both return early while D_8009B07B and D_8009B07C are both 1. */
void func_8005F3B8(s32 mode, s32 y, s32 a, s32 b, SVECTOR *offset);
void func_8005F714(s32 first, s32 second, s32 arg);

#endif
