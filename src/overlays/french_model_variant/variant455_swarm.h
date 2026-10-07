#ifndef FRENCH455_SWARM_VIEW_H
#define FRENCH455_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[64];
    SVECTOR b[64];
    SVECTOR c[64];
    SVECTOR d[64];
    u8 unknown_800[0x200];
    u8 color[4];
    u8 unknown_A04[0x10];
    s32 size[64];
    s32 done[64];
    u8 unknown_C14[0x100];
    s32 reset[64];
    MATRIX origins[64];
} Swarm455Group;

typedef struct {
    u8 unknown_00[0x1C];
    s32 count;
    u8 unknown_20[4];
    u32 rate;
    u8 unknown_28[4];
    u32 limit;
} Swarm455Timing;

typedef struct {
    Swarm455Group group;
    u8 unknown_1614[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2EC8[0x4C];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2F28[0x400];
    VECTOR directions[64];
    u8 unknown_3728[0x1C];
    u32 time;
    u8 unknown_3748[4];
    s32 step;
    u8 unknown_3750[4];
    Swarm455Timing *G32 timing;
    u8 unknown_3758[0x24];
    s32 phase;
} Swarm455State;

#endif
