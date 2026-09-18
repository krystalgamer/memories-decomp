#ifndef MEMORIES_DECOMP_FUNC_80058938_H
#define MEMORIES_DECOMP_FUNC_80058938_H

#include "../types.h"
#include "model.h"

/* Queues a tint ramp for one model slot. `part_mask` selects the model parts
   affected by the request; Model_QueueTintRequestForParts builds it from
   variadic part indices and func_80059AF8 passes 0 for every part. */
void Model_QueueTintRequest(
    s32 slot, s32 selection, ModelTintColor start, ModelTintColor end,
    s32 duration, const u8 *part_mask);
void Model_QueueTintRequestForParts(
    s32 slot, s32 part, ModelTintColor start, ModelTintColor end,
    s32 duration, ...);

/* Compatibility symbols for callers outside the matching C surface. */
void func_80058838(
    s32 slot, s32 part, ModelTintColor start, ModelTintColor end,
    s32 duration, ...);
void func_80058938(
    s32 slot, s32 selection, ModelTintColor start, ModelTintColor end,
    s32 duration, const u8 *part_mask);

#endif
