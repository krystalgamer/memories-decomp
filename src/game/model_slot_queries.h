#ifndef MEMORIES_DECOMP_MODEL_SLOT_QUERIES_H
#define MEMORIES_DECOMP_MODEL_SLOT_QUERIES_H

#include "../types.h"

/* Four reads of one model slot, each selected by slot index rather than by a
 * slot pointer, so a caller needs nothing of the record's layout.
 *
 * Model_GetSlotAnimationFrame returns the slot's 12.4 fixed-point animation
 * position shifted right by four.
 *
 * func_80058EC0 takes two steps: it reads the animation index at 0xBF5 that
 * Model_GetSlotAnimationIndex also returns, then uses it to index field_750
 * and hands back that row's `max`. Its return type is `u16` because `max` is one
 * (model.h:44); model.h:39-40 lists it among the four functions that read
 * the same halfword.
 *
 * Model_GetSlotAnimationSpeed, Model_GetSlotAnimationFrame and func_80058EC0
 * have no caller yet -- none in C and none in the generated assembly. They are
 * declared here anyway so that whoever decompiles their callers finds a
 * signature instead of inventing one. */
s32 Model_GetSlotAnimationSpeed(s32 index);
s32 Model_GetSlotAnimationIndex(s32 index);
s32 Model_GetSlotAnimationFrame(s32 index);
u16 func_80058EC0(s32 idx);

#endif
