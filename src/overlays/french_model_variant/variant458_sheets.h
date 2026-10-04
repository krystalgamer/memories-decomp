#ifndef FRENCH_MODEL_VARIANT458_SHEETS_H
#define FRENCH_MODEL_VARIANT458_SHEETS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
} Variant458Sheet;

typedef struct {
    u8 unknown_00[0x14];
    s32 scale_start;
    s32 scale_end;
    s32 move_start;
    s32 move_end;
    s32 fade_start;
} Variant458Timing;

typedef struct {
    u8 unknown_0000[0xC00];
    Variant458Sheet sheets[7];
    u8 unknown_0FB8[0xB68];
    POLY_GT4 quads[2];
    u8 unknown_1B88[0xD0];
    SVECTOR reference;
    MATRIX origins[7];
    VECTOR destinations[7];
    u8 unknown_1DB0[0x50];
    s32 factor;
    u8 unknown_1E04[4];
    s32 scale;
    u8 unknown_1E0C[0x14];
    VECTOR delta[7];
    u8 unknown_1E90[0x64];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1F00[12];
    s32 flags;
    s32 elapsed;
    u8 unknown_1F14[4];
    s32 step;
    u8 unknown_1F1C[8];
    Variant458Timing *G32 timing;
    u8 unknown_1F28[0x54];
    s32 phase;
} Variant458View;
#endif
