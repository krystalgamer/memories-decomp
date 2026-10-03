#ifndef FRENCH_MODEL_VARIANT373_POINTS_H
#define FRENCH_MODEL_VARIANT373_POINTS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[16];
    u8 unknown_80[0x20];
    SVECTOR rotation;
    s32 scale;
    u8 unknown_AC[8];
} Family373PointGroup;

typedef struct {
    u8 unknown_0000[0xD48];
    Family373PointGroup groups[3];
    u8 unknown_0F64[0x1400];
    POLY_FT4 quad;
    u8 unknown_238C[0x44];
    SVECTOR target;
    u8 unknown_23D8[0x14];
    s32 view_direction[3];
    u8 unknown_23F8[0x18];
    s32 step;
    u8 unknown_2414[0x70];
    s32 phase;
} Family373PointView;

void func_8013CEE4(u8 *context);
#endif
