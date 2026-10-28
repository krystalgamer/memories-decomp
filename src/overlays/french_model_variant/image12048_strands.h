#ifndef FRENCH_MODEL_IMAGE12048_STRANDS_H
#define FRENCH_MODEL_IMAGE12048_STRANDS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x28];
    s32 count;
    u8 unknown_2C[8];
    u32 start;
} Strands12048Timing;

typedef struct {
    SVECTOR pos[7];
    PSXLONG sxy[7];
    s32 angle[7];
    SVECTOR edge[7];
    PSXLONG edge_sxy[7];
    s32 width[7];
    CVECTOR color;
    u8 unknown_E4[0x64];
    s32 depth[7];
    s16 dx[7];
    s16 dy[7];
} Strands12048Strand;

typedef struct {
    s32 vx, vy, vz;
    u8 unknown_0C[0x14];
} Strands12048Origin;

typedef struct {
    s32 vx, vy, vz;
    s32 pad;
} Strands12048Reach;

typedef struct {
    Strands12048Strand strands[5];
    u8 unknown_780[0x934];
    POLY_FT4 polys[6];
    u8 unknown_11A4[0x4C];
    Strands12048Origin origins[5];
    u8 unknown_1290[0x3C];
    Strands12048Reach reach[5];
    u8 unknown_131C[0x18];
    s32 target[3];
    u8 unknown_1340[0xC];
    u32 frame;
    u32 time;
    u8 unknown_1354[4];
    u32 step;
    u8 unknown_135C[4];
    u32 span;
    u8 unknown_1364[4];
    Strands12048Timing *G32 timing;
    u8 unknown_136C[0x1C];
    s16 length[10];
    s16 mode[5];
    s16 fade[5];
    u8 unknown_13B0[4];
    s32 width;
    s32 sway;
    s32 twist;
    s32 phase;
} Strands12048State;

#endif
