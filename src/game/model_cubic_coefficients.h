#ifndef MEMORIES_DECOMP_MODEL_CUBIC_COEFFICIENTS_H
#define MEMORIES_DECOMP_MODEL_CUBIC_COEFFICIENTS_H

#include "../types.h"
#include "model_keyframe_spline.h"

void Model_BuildCubicCoefficients(
    s32 a, s32 b, s32 c, s32 d, Coeff *out, s32 scale
);

#endif
