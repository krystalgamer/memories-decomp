#ifndef FRENCH_MODEL_VARIANT478_SWARM_H
#define FRENCH_MODEL_VARIANT478_SWARM_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0xA28];
    ModelVariantSwarmGroupShort group;
    u8 unknown_103C[0x190C];
    POLY_FT4 polygon;
    u8 unknown_2970[0x38];
    s32 origin[3];
    u8 unknown_29B4[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_29D0[0x1C];
    u32 time;
    u8 unknown_29F0[4];
    s32 step;
    u8 unknown_29F8[4];
    ModelVariantQuadTimingLong *timing;
    u8 unknown_2A00[0x24];
    s32 phase;
} QuadSwarm478State;

#endif
