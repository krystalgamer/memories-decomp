#ifndef FRENCH447_SWARM_VIEW_H
#define FRENCH447_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantSwarmGroup group;
    u8 unknown_0714[0x180C];
    POLY_FT4 polygon;
    u8 unknown_1F48[0x38];
    s32 origin[3];
    u8 unknown_1F8C[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_1FA8[0x10];
    ModelVariantClock clock;
    ModelVariantQuadTimingLong *timing;
    u8 unknown_1FD8[0x20];
    s32 phase;
} Swarm447State;

#endif
