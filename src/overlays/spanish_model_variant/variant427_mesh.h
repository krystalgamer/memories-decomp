#ifndef SPANISH_MODEL_VARIANT427_MESH_H
#define SPANISH_MODEL_VARIANT427_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
    s32 size;
} Mesh427;

typedef struct {
    u8 unknown_000[0x240];
    s32 progress;
    u8 unknown_244[0x164];
} Mesh427Companion;

typedef struct {
    Mesh427 meshes[3];
    Mesh427Companion companions[3];
    u8 unknown_19C8[0x1A88];
    POLY_GT4 polygon;
    u8 unknown_3484[0x20C];
    VECTOR origins[3];
    u8 unknown_36C0[0x7C];
    s32 frame;
    u8 unknown_3740[8];
    s32 step;
    u8 unknown_374C[0x24];
    s32 size;
    u8 unknown_3774[0xC];
    s32 intensity;
    u8 unknown_3784[0x40];
    s32 phase;
} Mesh427State;

#endif
