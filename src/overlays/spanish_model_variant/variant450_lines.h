#ifndef SPANISH_MODEL_VARIANT450_LINES_H
#define SPANISH_MODEL_VARIANT450_LINES_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[9];
    u8 color[3];
    u8 unknown_4B[29];
} Variant450Group;

typedef struct {
    s32 threshold;
    u8 unknown_04[36];
    VECTOR translation;
    u8 unknown_38[480];
} Variant450Primary;

typedef struct {
    s32 scale;
    u8 unknown_04[12];
    s32 fading;
    s32 brightness;
    u8 unknown_18[136];
} Variant450Fade;

typedef struct {
    Variant450Group groups[6];
    u8 unknown_270[0x1D0];
    Variant450Primary primary[6];
    u8 unknown_10D0[0x25F0];
    Variant450Fade fade[6];
    u8 unknown_3A80[0x7C0];
    GsGLINE line;
    u8 unknown_4254[0x2C];
    s32 field_4280;
    s32 field_4284;
    u8 unknown_4288[4];
    s16 field_428C;
    s16 field_428E;
    s32 field_4290;
    s32 field_4294;
    s32 field_4298;
    u8 unknown_429C[12];
    s32 field_42A8;
    u8 unknown_42AC[8];
    s32 field_42B4;
    u8 unknown_42B8[0x3C];
    s32 field_42F4;
} Variant450View;

#endif
