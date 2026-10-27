#ifndef FRENCH_MODEL_VARIANT373_LAYERS_H
#define FRENCH_MODEL_VARIANT373_LAYERS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[51];
    SVECTOR rotation;
    s32 scale, field_1A4, done;
} Family373Layer;

typedef struct {
    u8 unknown_00[0x18];
    u32 start;
} Family373LayerConfig;

typedef struct {
    u8 unknown_0000[0x4EC];
    Family373Layer layers[5];
    u8 unknown_0D48[0x1118];
    POLY_GT4 quads[16];
    u8 unknown_21A0[0x224];
    s32 origin[3];
    SVECTOR target;
    s32 delta[3];
    u8 unknown_23E4[0x24];
    u32 elapsed;
    u32 field_240C;
    s32 step;
    u32 field_2414;
    Family373LayerConfig *G32 config;
    u8 unknown_241C[0x14];
    s32 scale;
    u8 unknown_2434[0x50];
    s32 phase;
} Family373LayerView;

void func_8013C0B0(u8 *context);

#endif
