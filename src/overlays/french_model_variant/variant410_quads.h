#ifndef FRENCH_MODEL_VARIANT410_QUADS_H
#define FRENCH_MODEL_VARIANT410_QUADS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][10];
    u8 unknown_140[0x50];
    CVECTOR color;
    s32 scale[10];
    s32 angle[10];
    s32 done[10];
    u8 unknown_20C[0xF0];
} Family410QuadRecord;

typedef struct {
    Family410QuadRecord record;
    u8 unknown_02FC[0x1D2C];
    POLY_FT4 quad;
    u8 unknown_2050[0x140];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_21A4[4];
    s16 screen_delta[2];
    u8 unknown_21AC[0x24];
    s32 step;
    u8 unknown_21D4[0x1C];
    s32 phase;
} Family410QuadView;

void func_8013BAAC(u8 *context);
#endif
