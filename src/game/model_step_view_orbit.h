#ifndef MEMORIES_DECOMP_MODEL_STEP_VIEW_ORBIT_H
#define MEMORIES_DECOMP_MODEL_STEP_VIEW_ORBIT_H

#include "../types.h"

/* The fifth argument is stored by every caller but never read. */
void Model_StepViewOrbit(s32 direction, s32 yaw, s32 pitch, s32 distance, s32 unused);

#endif
