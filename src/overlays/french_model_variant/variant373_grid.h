#ifndef FRENCH_MODEL_VARIANT373_GRID_H
#define FRENCH_MODEL_VARIANT373_GRID_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[9][17];
    CVECTOR colors[9];
} Family373Grid;

typedef struct {
    u8 unknown_00[0x14];
    u32 scale_start;
    u32 scale_end;
    u32 translation_start;
    u32 translation_end;
    u32 expansion_start;
} Family373GridConfig;

typedef struct {
    Family373Grid grid;
    u8 unknown_04EC[0x1CB4];
    POLY_GT4 quads[8];
    u8 unknown_2340[0x84];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[0x24];
    u32 elapsed;
    u8 unknown_240C[4];
    s32 step;
    u8 unknown_2414[4];
    Family373GridConfig *G32 config;
    u8 unknown_241C[0x14];
    s32 scale;
    s32 translation;
    s32 rotation;
    s32 oscillation;
    s32 brightness;
    u8 unknown_2444[0x3C];
    s32 texture_offset;
    s32 phase;
    u8 unknown_2488[0x14];
    s16 orientation;
} Family373GridView;

void func_8013C76C(u8 *context);
#endif
