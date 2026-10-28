#ifndef FRENCH_MODEL_IMAGE134622_SWARM_H
#define FRENCH_MODEL_IMAGE134622_SWARM_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[16];
    SVECTOR b[16];
    SVECTOR c[16];
    SVECTOR d[16];
    u8 unknown_200[0x80];
    u8 color[4];
    u8 unknown_284[0x10];
    s32 size[16];
    s32 done[16];
    u8 unknown_314[0x40];
    s32 reset[16];
    s32 fade[16];
    MATRIX origins[16];
} Swarm134622Group;

typedef struct {
    u8 unknown_00[0x1C];
    s32 count;
    u8 unknown_20[4];
    u32 rate;
    u8 unknown_28[4];
    u32 limit;
} Swarm134622Timing;

typedef struct {
    Swarm134622Group group;
    u8 unknown_5D4[0x188C];
    POLY_FT4 polygon;
    u8 unknown_1E88[0x54];
    VECTOR direction;
    VECTOR drift;
    DVECTOR projected;
    u8 unknown_1F00[0x100];
    VECTOR directions[16];
    u8 unknown_2100[0x1C];
    u32 time;
    u8 unknown_2120[4];
    s32 step;
    u8 unknown_2128[4];
    Swarm134622Timing *G32 timing;
    u8 unknown_2130[0x18];
    s32 spin;
    s32 drift_scale;
    u8 unknown_2150[0x10];
    s32 phase;
} Swarm134622State;

#endif
