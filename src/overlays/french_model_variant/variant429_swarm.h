#ifndef FRENCH429_SWARM_VIEW_H
#define FRENCH429_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[6];
    SVECTOR b[6];
    SVECTOR c[6];
    SVECTOR d[6];
    u8 unknown_0C0[0x30];
    u8 color[4];
    u8 unknown_0F4[0x10];
    s32 size[6];
    s32 done[6];
    u8 unknown_134[0x18];
    s32 reset[6];
} Swarm429Group;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Swarm429Timing;

typedef struct {
    u8 unknown_0000[0x2FC];
    Swarm429Group group;
    u8 unknown_0460[0x188C];
    POLY_FT4 polygon;
    u8 unknown_1D14[0x38];
    s32 origin[3];
    u8 unknown_1D58[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_1D74[0x60];
    VECTOR directions[6];
    u8 unknown_1E34[0x1C];
    u32 time;
    u8 unknown_1E54[4];
    s32 step;
    u8 unknown_1E5C[4];
    Swarm429Timing *G32 timing;
    u8 unknown_1E64[0x24];
    s32 phase;
} Swarm429State;

#endif
