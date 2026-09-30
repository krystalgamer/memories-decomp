#ifndef MEMORIES_MODEL_VARIANT408_UPDATE_H
#define MEMORIES_MODEL_VARIANT408_UPDATE_H

#include "../../types.h"
#include "variant408_draw.h"
#include "../../game/gpu_packets.h"

typedef struct {
    SVECTOR points[9];
} ModelVariant408MeshRow;

typedef struct {
    ModelVariant408MeshRow rows[17];
    CVECTOR colors[9];
    VECTOR scale;
    s32 phases[17];
    s32 repeats[17];
} ModelVariant408Mesh;

typedef struct {
    u8 unknown_00[12];
    u32 start;
    u32 full;
    u32 cycle_start;
    u8 unknown_18[12];
} ModelVariant408UpdateConfig;

typedef struct {
    ModelVariant408Mesh meshes[1];
    ModelVariant408Ring rings[4];
    u8 unknown_BF4[28];
    POLY_G4 quad;
    u8 unknown_C34[0x130];
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
    ModelVariant408UpdateConfig *G32 config;
    u8 unknown_DE0[8];
    s32 extension;
    s32 angle;
    s32 repeat_limit;
    u8 unknown_DF4[12];
    s32 state;
    u8 unknown_E04[0x10];
    s16 slot;
} ModelVariant408UpdateState;

#endif
