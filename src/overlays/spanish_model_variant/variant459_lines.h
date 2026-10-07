#ifndef SPANISH_MODEL_VARIANT459_LINES_H
#define SPANISH_MODEL_VARIANT459_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    u8 color[4];
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines459;

typedef struct {
    u8 unknown_00[0x34];
    u32 cycle_start;
    u32 cycle_end;
} Lines459Timing;

typedef struct {
    u8 unknown_0000[0xCA8];
    Lines459 groups[3];
    u8 unknown_1188[0x1D30];
    GsGLINE line;
    u8 unknown_2ECC[0x14];
    s32 origin[3];
    u8 unknown_2EEC[0x160];
    SVECTOR target;
    u8 unknown_3054[4];
    s32 direction_a[3];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_3074[0x10];
    u32 time;
    u8 unknown_3088[4];
    u32 step;
    u8 unknown_3090[0xC];
    Lines459Timing *G32 timing;
    u8 unknown_30A0[0x3C];
    s32 phase;
} Lines459State;

#endif
