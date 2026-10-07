#ifndef FRENCH_MODEL_VARIANT478_SWARM_H
#define FRENCH_MODEL_VARIANT478_SWARM_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One group of 32 quads: corners, a shared colour, growth and done flags. */
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
} QuadSwarm478Group;

typedef struct {
    u8 unknown_00[0x24];
    u32 limit;
} QuadSwarm478Timing;

typedef struct {
    u8 unknown_0000[0xA28];
    QuadSwarm478Group group;
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
    QuadSwarm478Timing *G32 timing;
    u8 unknown_2A00[0x24];
    s32 phase;
} QuadSwarm478State;

#endif
