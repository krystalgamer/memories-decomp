#ifndef FRENCH_MODEL_VARIANT383_STRIP_H
#define FRENCH_MODEL_VARIANT383_STRIP_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[3][2];
    PSXLONG projected[3][2];
    u8 unknown_48[0x1C];
    PSXLONG depth[2];
} Family383Strip;

typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
} Family383StripConfig;

typedef struct {
    Family383Strip strip;
    u8 unknown_006C[0x1320];
    POLY_GT4 quad;
    u8 unknown_13C0[0x124];
    s32 origin[3];
    u8 unknown_14F0[8];
    s32 delta[3];
    u8 unknown_1504[4];
    s16 angle_delta[2];
    u8 unknown_150C[0x24];
    s32 step;
    u8 unknown_1534[4];
    Family383StripConfig *G32 config;
    u8 unknown_153C[0x10];
    s32 index_154C;
    u8 unknown_1550[0x28];
    s16 factor;
    s16 width;
    u8 unknown_157C[4];
    s32 phase;
} Family383StripView;

void func_8013BCBC(u8 *context);
#endif
