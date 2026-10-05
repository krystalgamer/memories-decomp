#ifndef SPANISH_MODEL_VARIANT385_STRIP_H
#define SPANISH_MODEL_VARIANT385_STRIP_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR upper[3];
    SVECTOR center[3];
    SVECTOR lower[3];
    PSXLONG projected[3][3];
    u8 unknown_6C[8];
    PSXLONG flags[3];
    s32 depth[3];
} Strip385;

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
} Strip385Timing;

typedef struct {
    u8 unknown_00[0x4E0];
    Strip385 strips[1];
    u8 unknown_56C[0x8C0];
    POLY_GT4 polygon;
    u8 unknown_E60[0x14C];
    s32 origin[3];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_FCC[4];
    DVECTOR projected;
    u8 unknown_FD4[0x18];
    u32 frame;
    u32 time;
    u8 unknown_FF4[0xC];
    Strip385Timing *G32 timing;
    u8 unknown_1004[0x10];
    s32 iteration;
    u8 unknown_1018[0x10];
    s16 radius;
    s16 progress;
    u8 unknown_102C[4];
    s32 phase;
} Strip385State;

#endif
