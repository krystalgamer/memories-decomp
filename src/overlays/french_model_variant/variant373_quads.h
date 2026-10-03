#ifndef FRENCH_MODEL_VARIANT373_QUADS_H
#define FRENCH_MODEL_VARIANT373_QUADS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
} Family373QuadGroup;

typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
    u8 unknown_0E[6];
    u32 duration_14;
} Family373QuadConfig;

typedef struct {
    u8 unknown_0000[0xFD0];
    Family373QuadGroup group;
    u8 unknown_1058[0xDD4];
    POLY_GT4 quad;
    u8 unknown_1E60[0x564];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[0x20];
    s32 flags;
    u32 elapsed;
    u8 unknown_240C[4];
    s32 step;
    u8 unknown_2414[4];
    Family373QuadConfig *G32 config;
    u8 unknown_241C[0x10];
    s32 index_242C;
    u8 unknown_2430[0x14];
    s32 scale;
    s32 brightness;
    u8 unknown_244C[0x28];
    s16 factor;
    u8 unknown_2476[0xE];
    s32 phase;
} Family373QuadView;

void func_8013DE68(u8 *context);
#endif
