#ifndef FRENCH428_SWARM_VIEW_H
#define FRENCH428_SWARM_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[32];
    SVECTOR b[32];
    SVECTOR c[32];
    SVECTOR d[32];
    u8 unknown_400[0x100];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32];
    s32 done[32];
    u8 unknown_614[0x80];
    s32 reset[32];
} Swarm428Group;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Swarm428Timing;

typedef struct {
    u8 unknown_0000[0x2FC];
    Swarm428Group group;
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
    Swarm428Timing *G32 timing;
    u8 unknown_2754[0x24];
    s32 phase;
} Swarm428State;

#endif
