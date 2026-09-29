#ifndef MEMORIES_MODEL_VARIANT432_DRAW_H
#define MEMORIES_MODEL_VARIANT432_DRAW_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
    s32 scale;
    u8 unknown_08C[12];
} ModelVariant432Ring;

typedef struct {
    u8 unknown_000[0x15C];
    ModelVariant432Ring rings[2];
    u8 unknown_28C[0x28D0];
    POLY_GT4 quad;
    u8 unknown_2B90[0x3D4];
    SVECTOR position;
    u8 unknown_2F6C[0x130];
    s32 frame;
    u8 unknown_30A0[8];
    s32 step;
    u8 unknown_30AC[0x3C];
    s32 state;
} ModelVariant432State;

#endif
