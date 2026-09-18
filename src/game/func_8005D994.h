#ifndef MEMORIES_DECOMP_FUNC_8005D994_H
#define MEMORIES_DECOMP_FUNC_8005D994_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Builds the two four-halfword vectors a model effect is positioned with and
 * hands them on. `offset` is optional: when it is present, its vx/vy/vz are
 * added to the first vector, with vx and vz negated according to the sign
 * of arg0.
 *
 * It is an SVECTOR. model_intro_controller.c passes the address of its
 * SVECTOR offset local at both call sites, and the body reads exactly an
 * SVECTOR's first three halfwords. func_8005F5C8 in model_effect_state.c
 * forwards an s32 it received from unmatched assembly, and casts it. */
void func_8005D994(
    s32 arg0, s32 arg1, s32 arg2, s32 arg3, SVECTOR *offset, s32 arg5
);

#endif
