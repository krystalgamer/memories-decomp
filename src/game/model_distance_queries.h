#ifndef MEMORIES_DECOMP_MODEL_DISTANCE_QUERIES_H
#define MEMORIES_DECOMP_MODEL_DISTANCE_QUERIES_H

#include "../types.h"

s32 Model_GetSlotDistanceFromCameraEye(s32 index);
s32 Model_GetSlotDistanceFromCameraTarget(s32 index);

/* The unmatched model controller and its tracked candidate still relocate
 * against the original address symbol. c_symbols.ld aliases it to the named
 * matching definition above. */
s32 func_8005A2E0(s32 index);

#endif
