#ifndef FRENCH_MODEL_VARIANT383_QUADS_H
#define FRENCH_MODEL_VARIANT383_QUADS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
} Family383QuadGroup;

typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
    u8 unknown_0E[2];
    u32 duration_10;
} Family383QuadConfig;

typedef struct {
    u8 unknown_0000[0x6C];
    Family383QuadGroup group;
    u8 unknown_00F4[0x1298];
    POLY_GT4 quads[2];
    u8 unknown_13F4[0xF0];
    s32 origin[3];
    u8 unknown_14F0[8];
    s32 delta[3];
    u8 unknown_1504[0x20];
    s32 flags;
    u32 elapsed;
    u8 unknown_152C[4];
    s32 step;
    u8 unknown_1534[4];
    Family383QuadConfig *G32 config;
    u8 unknown_153C[0x10];
    s32 index_154C;
    s32 scale;
    s32 brightness;
    u8 unknown_1558[0x20];
    s16 factor;
    u8 unknown_157A[6];
    s32 phase;
} Family383QuadView;

void func_8013C91C(u8 *context);
#endif
