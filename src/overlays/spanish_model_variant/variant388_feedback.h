#ifndef SPANISH_MODEL_VARIANT388_FEEDBACK_H
#define SPANISH_MODEL_VARIANT388_FEEDBACK_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[3][17];
    CVECTOR inner;
    CVECTOR outer;
    s32 progress;
    u8 unknown_1A4[4];
} Feedback388;

typedef struct {
    u8 unknown_00[0x5C8];
    Feedback388 groups[5];
    u8 unknown_E10[0x94C];
    POLY_GT4 quad;
    u8 unknown_1790[0x94];
    SVECTOR origin;
    VECTOR direction;
    u8 unknown_183C[0x28];
    s32 step;
    u8 unknown_1868[0x30];
    s32 phase;
} Feedback388State;

#endif
