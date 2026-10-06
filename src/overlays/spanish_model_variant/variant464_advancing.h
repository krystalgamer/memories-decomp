#ifndef SPANISH_MODEL_VARIANT464_ADVANCING_H
#define SPANISH_MODEL_VARIANT464_ADVANCING_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[6];
    PSXLONG projected[3][2];
    CVECTOR inner[2];
    CVECTOR outer[2];
    VECTOR scale;
    SVECTOR rotation;
    s32 depth[2];
    s32 progress[2];
    s32 active;
} Advancing464;

typedef struct {
    u8 unknown_00[0x44];
    s32 count;
} Advancing464Timing;

typedef struct {
    u8 unknown_00[0x4E0];
    Advancing464 groups[5];
    u8 unknown_774[0x130C];
    POLY_GT4 quad;
    u8 unknown_1AB4[0x124];
    s32 origin[3];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_1BF8[4];
    DVECTOR projected;
    u8 unknown_1C00[0x18];
    u32 frame;
    u32 time;
    u8 unknown_1C20[4];
    u32 step;
    u8 unknown_1C28[4];
    Advancing464Timing *G32 timing;
    u8 unknown_1C30[0x18];
    s16 width;
    u8 unknown_1C4A[0x16];
    s32 phase;
} Advancing464State;

#endif
