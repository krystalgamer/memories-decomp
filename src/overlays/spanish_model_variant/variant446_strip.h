#ifndef SPANISH_MODEL_VARIANT446_STRIP_H
#define SPANISH_MODEL_VARIANT446_STRIP_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR upper[5];
    SVECTOR center[5];
    SVECTOR lower[5];
    PSXLONG projected[3][5];
    u8 unknown_B4[8];
    s32 depth[5];
} Strip446;

typedef struct {
    u8 unknown_00[0xC];
    u16 iterations;
    u8 unknown_0E[2];
    s32 width;
    u8 unknown_14[0x1C];
    u32 start;
    u32 expanded;
    u32 fade;
    u32 end;
} Strip446Timing;

typedef struct {
    u8 unknown_0000[0x1060];
    Strip446 strips[1];
    u8 unknown_1130[0x880];
    POLY_GT4 polygon;
    u8 unknown_19E4[0x14C];
    s32 origin[3];
    u8 unknown_1B3C[0x28];
    s32 direction[3];
    u8 unknown_1B70[4];
    DVECTOR projected;
    u8 unknown_1B78[0x18];
    u32 frame;
    u32 time;
    u8 unknown_1B98[0xC];
    Strip446Timing *G32 timing;
    u8 unknown_1BA8[0x10];
    s32 iteration;
    u8 unknown_1BBC[0x10];
    s16 radius;
    s16 progress;
    u8 unknown_1BD0[0xC];
    s32 phase;
} Strip446State;

#endif
