#ifndef MEMORIES_DECOMP_MODEL_GET_FRAME_STEP_H
#define MEMORIES_DECOMP_MODEL_GET_FRAME_STEP_H

#include "../types.h"

/* Returns the frame step: the volatile counter at gGraphics_bFrameStep clamped to at
 * most 6. Callers scale an animation rate by it (m->playback_speed * step) or test
 * it directly against 2, so the result is consumed as a signed value even
 * though the definition computes it in an unsigned register. */
s32 Model_GetFrameStep(void);

#endif
