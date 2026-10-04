#ifndef FRENCH_MODEL_VARIANT373_RAYS_H
#define FRENCH_MODEL_VARIANT373_RAYS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    u8 unknown_00[0x10];
    SVECTOR points[2];
    PSXLONG projected[2];
    s32 angle[2];
    SVECTOR edges[2];
    PSXLONG edge_projected[2];
    s32 width[2];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[4];
    s32 depth[2];
    s16 x_offset[2];
    s16 y_offset[2];
    u8 unknown_74[8];
} Family373Ray;

typedef struct {
    u8 unknown_0000[0x15F8];
    Family373Ray rays[16];
    u8 unknown_1DB8[0x40];
    POLY_GT4 quad;
    u8 unknown_1E2C[0x598];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[8];
    s32 view_direction[3];
    u8 unknown_23F8[0xC];
    s32 flags;
    u8 unknown_2408[8];
    s32 step;
    u8 unknown_2414[0x30];
    s32 scale;
    s32 brightness;
    u8 unknown_244C[0x14];
    s16 angle_a;
    s16 angle_b;
    u8 unknown_2464[4];
    s16 radius;
    u8 unknown_246A[0xA];
    s16 factor;
    u8 unknown_2476[0xE];
    s32 phase;
} Family373RayView;

void func_8013E3EC(u8 *context);
#endif
