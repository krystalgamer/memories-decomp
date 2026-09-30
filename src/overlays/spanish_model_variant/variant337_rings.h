#ifndef MEMORIES_MODEL_VARIANT337_RINGS_H
#define MEMORIES_MODEL_VARIANT337_RINGS_H
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
} ModelVariant337Ring;

typedef struct {
    u8 unknown_00[16];
    u32 start;
    u32 end;
    u32 unknown_18;
    u32 fade_start;
    u32 fade_end;
} ModelVariant337Config;

typedef struct {
    u8 unknown_000[0x3A4];
    ModelVariant337Ring rings[2];
    u8 unknown_4D4[0x764];
    POLY_GT4 quad;
    u8 unknown_C6C[0x60];
    MATRIX transform;
    SVECTOR target;
    u8 unknown_CF4[0x2C];
    s32 frame;
    u32 elapsed;
    u32 unknown_D28;
    s32 step;
    u32 unknown_D30;
    ModelVariant337Config *config;
    u8 unknown_D38[0x2C];
    s32 angle;
    u32 unknown_D68;
    s32 state;
} ModelVariant337State;
#endif
