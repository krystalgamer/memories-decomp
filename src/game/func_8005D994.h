#ifndef MEMORIES_DECOMP_FUNC_8005D994_H
#define MEMORIES_DECOMP_FUNC_8005D994_H

#include "../types.h"

/* Builds the two four-halfword vectors a model effect is positioned with and
 * hands them on. `arg4` is an optional offset record: the body tests it
 * against null and, when it is present, adds three halfwords out of it at
 * +0, +2 and +4 to the first vector, negating the two signed ones according
 * to the sign of arg0.
 *
 * model_effect_state.c declared that parameter s32. It is a pointer -- the
 * definition compares it with `(u8 *)0` and dereferences it three times --
 * and its one caller, func_8005F5C8, holds the same value in an s32 and
 * passes it straight through. Nothing compared the two spellings, and an
 * integer standing in for a pointer is not a diagnosable mismatch on its
 * own, so the build matched with both live in the tree.
 *
 * The parameter stays u8 * rather than being given a struct: three halfwords
 * at fixed offsets is all the body evidences, func_8005F5C8's own argument
 * arrives from unmatched assembly so nothing in C says where it comes from,
 * and model_effect_state.h records the same u8 * choice for the same reason. */
void func_8005D994(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 *arg4, s32 arg5);

#endif
