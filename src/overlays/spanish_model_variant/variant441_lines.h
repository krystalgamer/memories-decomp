#ifndef SPANISH_MODEL_VARIANT441_LINES_H
#define SPANISH_MODEL_VARIANT441_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 size;
    u8 unknown_198[8];
} Lines441;

typedef struct {
    u8 unknown_00[0x1C];
    s32 start;
    s32 end;
} Lines441Timing;

typedef struct {
    u8 unknown_00[0xAF8];
    Lines441 groups[3];
    u8 unknown_FD8[0x1690];
    GsGLINE line;
    u8 unknown_267C[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_26A4[4];
    s32 direction_a[2];
    u8 unknown_26B0[4];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_26C4[0x10];
    s32 time;
    u8 unknown_26D8[4];
    s32 step;
    u8 unknown_26E0[4];
    Lines441Timing *G32 timing;
    u8 unknown_26E8[0x34];
    s32 phase;
} Lines441State;

#endif
