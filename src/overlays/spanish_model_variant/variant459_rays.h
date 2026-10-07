#ifndef SPANISH_MODEL_VARIANT459_RAYS_H
#define SPANISH_MODEL_VARIANT459_RAYS_H

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
} Rays459;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Rays459Pulse;

typedef struct {
    u8 unknown_00[0x24];
    u16 count;
} Rays459Timing;

typedef struct {
    u8 unknown_0000[0x1758];
    Rays459 rays[8];
    u8 unknown_1AB8[0xAB0];
    Rays459Pulse pulse;
    u8 unknown_25F4[0x72C];
    POLY_G3 triangle;
    u8 unknown_2D3C[0x1A4];
    s32 origin[3];
    u8 unknown_2EEC[0x168];
    s32 direction[3];
    u8 unknown_3060[8];
    s32 direction_b[3];
    u8 unknown_3074[0xC];
    s32 frame;
    u8 unknown_3084[8];
    s32 step;
    u8 unknown_3090[0xC];
    Rays459Timing *G32 timing;
    u8 unknown_30A0[0x1C];
    s16 index;
    u8 unknown_30BE[0x12];
    s32 progress;
    u8 unknown_30D4[4];
    s32 angle;
    u8 unknown_30DC[0x14];
    s16 mirror;
} Rays459State;

#endif
