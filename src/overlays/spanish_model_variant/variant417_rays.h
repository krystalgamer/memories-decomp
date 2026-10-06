#ifndef SPANISH_MODEL_VARIANT417_RAYS_H
#define SPANISH_MODEL_VARIANT417_RAYS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2];
    PSXLONG screen[2];
    s32 angles[2];
    SVECTOR offset_points[2];
    PSXLONG offset_screen[2];
    s32 projected_width[2];
    u8 unknown_40[0x1C];
    s32 depth[2];
    s16 width_x[2];
    s16 width_y[2];
} Rays417;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Rays417Pulse;

typedef struct {
    u8 unknown_00[0x18];
    u16 count;
} Rays417Timing;

typedef struct {
    u8 unknown_00[0xA98];
    Rays417Pulse pulse;
    u8 unknown_B24[0xA4];
    Rays417 rays[8];
    u8 unknown_F28[0x1058];
    POLY_G3 triangle;
    u8 unknown_1F9C[0x148];
    s32 origin[3];
    u8 unknown_20F0[8];
    s32 direction[3];
    u8 unknown_2104[8];
    s32 direction_b[3];
    u8 unknown_2118[0xC];
    s32 frame;
    u8 unknown_2128[8];
    s32 step;
    u8 unknown_2134[4];
    Rays417Timing *G32 timing;
    u8 unknown_213C[0x10];
    s32 index;
    u8 unknown_2150[0x18];
    s16 progress;
    u8 unknown_216A[2];
    s32 angle;
    u8 unknown_2170[0x14];
    s16 mirror;
} Rays417State;

#endif
