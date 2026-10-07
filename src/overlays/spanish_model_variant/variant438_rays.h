#ifndef SPANISH_MODEL_VARIANT438_RAYS_H
#define SPANISH_MODEL_VARIANT438_RAYS_H

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
} Rays438;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Rays438Pulse;

typedef struct {
    u8 unknown_00[0x18];
    u16 count;
} Rays438Timing;

typedef struct {
    u8 unknown_0000[0x1410];
    Rays438 rays[8];
    u8 unknown_1770[0x1C8];
    Rays438Pulse pulse;
    u8 unknown_19C4[0x6D4];
    POLY_G3 triangle;
    u8 unknown_20B4[0x1A4];
    s32 origin[3];
    u8 unknown_2264[8];
    s32 direction[3];
    u8 unknown_2278[8];
    s32 direction_b[3];
    u8 unknown_228C[0xC];
    s32 frame;
    u8 unknown_229C[8];
    s32 step;
    u8 unknown_22A8[0xC];
    Rays438Timing *G32 timing;
    u8 unknown_22B8[0x10];
    s16 index;
    u8 unknown_22CA[0x12];
    s32 progress;
    u8 unknown_22E0[4];
    s32 angle;
    u8 unknown_22E8[0x14];
    s16 mirror;
} Rays438State;

#endif
