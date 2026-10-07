#ifndef FRENCH447_SWARM_VIEW_H
#define FRENCH447_SWARM_VIEW_H

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
} Swarm447Group;

typedef struct {
    u8 unknown_00[0x24];
    u32 limit;
} Swarm447Timing;

typedef struct {
    Swarm447Group group;
    u8 unknown_0714[0x180C];
    POLY_FT4 polygon;
    u8 unknown_1F48[0x38];
    s32 origin[3];
    u8 unknown_1F8C[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_1FA8[0x1C];
    u32 time;
    u8 unknown_1FC8[4];
    s32 step;
    u8 unknown_1FD0[4];
    Swarm447Timing *G32 timing;
    u8 unknown_1FD8[0x20];
    s32 phase;
} Swarm447State;

#endif
