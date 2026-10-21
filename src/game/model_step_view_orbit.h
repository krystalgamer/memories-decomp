#ifndef MEMORIES_DECOMP_MODEL_STEP_VIEW_ORBIT_H
#define MEMORIES_DECOMP_MODEL_STEP_VIEW_ORBIT_H

#include "../types.h"

/* Retail callers store an unused fifth argument on the stack. Keep that
 * call-site contract without adding a parameter the callee never reads. */
#ifdef FUNC_80058434_CALL_WITH_UNUSED_ARG
void Model_StepViewOrbit(s32 direction, s32 yaw, s32 pitch, s32 distance, s32 unused);
#else
void Model_StepViewOrbit(s32 direction, s32 yaw, s32 pitch, s32 distance);
#endif

#endif
