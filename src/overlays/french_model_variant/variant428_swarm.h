#ifndef FRENCH428_SWARM_VIEW_H
#define FRENCH428_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0x2FC];
    ModelVariantSwarmGroup group;
    u8 unknown_0A10[0x188C];
    POLY_FT4 polygon;
    u8 unknown_22C4[0x38];
    s32 origin[3];
    u8 unknown_2308[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2324[0x200];
    VECTOR directions[32];
    u8 unknown_2724[0x1C];
    u32 time;
    u8 unknown_2744[4];
    s32 step;
    u8 unknown_274C[4];
    ModelVariantQuadTiming *timing;
    u8 unknown_2754[0x24];
    s32 phase;
} Swarm428State;

#endif
