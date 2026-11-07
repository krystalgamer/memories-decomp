#ifndef FRENCH419_SWARM_VIEW_H
#define FRENCH419_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1C];
    u16 spin;
    u8 unknown_1E[6];
    u32 limit;
} Swarm419Timing;

typedef struct {
    u8 unknown_0000[0x2FC];
    ModelVariantSwarmGroupShort group;
    u8 unknown_0910[0x190C];
    POLY_FT4 polygon;
    u8 unknown_2244[0x38];
    s32 origin[3];
    u8 unknown_2288[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_22A4[0x1C];
    u32 time;
    u8 unknown_22C4[4];
    s32 step;
    u8 unknown_22CC[4];
    Swarm419Timing *timing;
    u8 unknown_22D4[0x20];
    s32 phase;
} Swarm419State;

#endif
