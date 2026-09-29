#ifndef MEMORIES_MODEL_VARIANT408_DRAW_H
#define MEMORIES_MODEL_VARIANT408_DRAW_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    VECTOR position;
    s32 phase;
    s32 repeats;
} ModelVariant408Ring;

typedef struct {
    u8 unknown_000[0x584];
    ModelVariant408Ring rings[4];
    u8 unknown_BF4[0x170];
    GsGLINE line;
    MATRIX matrix;
    SVECTOR target;
    VECTOR delta;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    s32 frame;
    s32 elapsed;
    s32 animation_frame;
    s32 step;
    u8 unknown_DDC[0x24];
    s32 state;
    u8 unknown_E04[0x10];
    s16 slot;
} ModelVariant408State;

#endif
