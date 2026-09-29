#ifndef MEMORIES_MODEL_VARIANT432_BANDS_H
#define MEMORIES_MODEL_VARIANT432_BANDS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[2][17];
    CVECTOR color[2];
    s32 phase;
    u8 unknown_11C[4];
} ModelVariant432Band;

typedef struct {
    u8 unknown_000[0x5EC];
    ModelVariant432Band bands[3];
    u8 unknown_94C[0x2244];
    POLY_GT4 quads[16];
    u8 unknown_2ED0[0x94];
    SVECTOR position;
    u8 unknown_2F6C[0x13C];
    s32 step;
    u8 unknown_30AC[0x3C];
    s32 state;
} ModelVariant432BandsState;

#endif
