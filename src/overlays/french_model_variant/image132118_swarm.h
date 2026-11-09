#ifndef FRENCH_MODEL_IMAGE132118_SWARM_H
#define FRENCH_MODEL_IMAGE132118_SWARM_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantSwarmGroup groups[3];
    u8 unknown_153C[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2DF0[0x44];
    MATRIX origins[3];
    u8 unknown_2E94[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2EB0[0x600];
    VECTOR directions[3][32];
    u8 unknown_3AB0[0x10];
    ModelVariantClock clock;
    ModelVariantQuadTiming *timing;
    u8 unknown_3AE0[0x24];
    s32 phase;
} Swarm132118State;

#endif
