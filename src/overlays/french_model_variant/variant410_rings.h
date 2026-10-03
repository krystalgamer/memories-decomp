#ifndef FRENCH_MODEL_VARIANT410_RINGS_H
#define FRENCH_MODEL_VARIANT410_RINGS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR inner;
    CVECTOR outer;
    s32 scale;
    u8 unknown_8C[12];
} Family410Ring;

typedef struct {
    u8 unknown_00[12];
    u32 duration;
} Family410Config;

typedef struct {
    u8 unknown_0000[0x1EF8];
    Family410Ring rings[2];
    u8 unknown_2028[0x9C];
    POLY_GT4 quad;
    u8 unknown_20F8[0x8C];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_2198[0x2C];
    s32 frame;
    u32 elapsed;
    s32 animation_frame;
    s32 step;
    s32 fade;
    Family410Config *G32 config;
    u8 unknown_21DC[0x14];
    s32 phase;
} Family410RingView;

void func_8013C8B8(u8 *context);
#endif
