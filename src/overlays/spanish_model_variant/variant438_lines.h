#ifndef SPANISH_MODEL_VARIANT438_LINES_H
#define SPANISH_MODEL_VARIANT438_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    u8 color[4];
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines438;

typedef struct {
    u8 unknown_00[0x1C];
    u32 cycle_start;
    u32 cycle_end;
} Lines438Timing;

typedef struct {
    u8 unknown_0000[0x960];
    Lines438 groups[3];
    u8 unknown_0E40[0x13F0];
    GsGLINE line;
    u8 unknown_2244[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_226C[4];
    s32 direction_a[3];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_228C[0x10];
    u32 time;
    u8 unknown_22A0[4];
    u32 step;
    u8 unknown_22A8[0xC];
    Lines438Timing *G32 timing;
    u8 unknown_22B8[0x30];
    s32 phase;
} Lines438State;

#endif
