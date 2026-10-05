#ifndef SPANISH_MODEL_VARIANT426_MESH_H
#define SPANISH_MODEL_VARIANT426_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
} Mesh426Rows;

typedef struct {
    Mesh426Rows mesh;
    u8 unknown_04EC[0x1960];
    POLY_GT4 polygon;
    u8 unknown_1E80[0x248];
    SVECTOR origin;
    s32 direction[3];
    u8 unknown_20DC[0x20];
    s32 frame;
    u8 unknown_2100[8];
    s32 step;
    u8 unknown_210C[0x1C];
    s32 size;
    u8 unknown_212C[0xC];
    s32 intensity;
    u8 unknown_213C[0x44];
    s32 phase;
} Mesh426State;

#endif
