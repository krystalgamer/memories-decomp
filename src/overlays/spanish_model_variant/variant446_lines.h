#ifndef SPANISH_MODEL_VARIANT446_LINES_H
#define SPANISH_MODEL_VARIANT446_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines446;

typedef struct {
    u8 unknown_00[0x30];
    u32 duration;
} Lines446Timing;

typedef struct {
    u8 unknown_0000[0xB80];
    Lines446 groups[3];
    u8 unknown_1060[0xAA8];
    GsGLINE line;
    u8 unknown_1B1C[0x14];
    s32 origin[3];
    u8 unknown_1B3C[0x20];
    SVECTOR target;
    u8 unknown_1B64[4];
    s32 direction_a[2];
    u8 unknown_1B70[4];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_1B84[0x10];
    u32 time;
    u8 unknown_1B98[4];
    s32 step;
    u8 unknown_1BA0[4];
    Lines446Timing *G32 timing;
    u8 unknown_1BA8[0x34];
    s32 phase;
} Lines446State;

#endif
