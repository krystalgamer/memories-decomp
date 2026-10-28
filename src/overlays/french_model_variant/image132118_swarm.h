#ifndef FRENCH_MODEL_IMAGE132118_SWARM_H
#define FRENCH_MODEL_IMAGE132118_SWARM_H

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
} Swarm132118Group;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Swarm132118Timing;

typedef struct {
    Swarm132118Group groups[3];
    u8 unknown_153C[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2DF0[0x44];
    MATRIX origins[3];
    u8 unknown_2E94[8];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2EB0[0x600];
    VECTOR directions[3][32];
    u8 unknown_3AB0[0x1C];
    u32 time;
    u8 unknown_3AD0[4];
    s32 step;
    u8 unknown_3AD8[4];
    Swarm132118Timing *G32 timing;
    u8 unknown_3AE0[0x24];
    s32 phase;
} Swarm132118State;

#endif
