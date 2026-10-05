#ifndef SPANISH_MODEL_VARIANT385_LINES_H
#define SPANISH_MODEL_VARIANT385_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines385;

typedef struct {
    u8 unknown_00[0x30];
    u32 duration;
} Timing385;

typedef struct {
    Lines385 groups[3];
    u8 unknown_4E0[0xAA4];
    GsGLINE line;
    u8 unknown_F98[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_FC0[4];
    s32 direction_a[2];
    u8 unknown_FCC[4];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_FE0[0x10];
    u32 time;
    u8 unknown_FF4[4];
    s32 step;
    u8 unknown_FFC[4];
    Timing385 *G32 timing;
    u8 unknown_1004[0x2C];
    s32 phase;
} State385;

#endif
