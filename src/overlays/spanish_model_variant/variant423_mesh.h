#ifndef SPANISH_MODEL_VARIANT423_MESH_H
#define SPANISH_MODEL_VARIANT423_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/gpu_packets.h"

typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
} Mesh423Rows;

typedef struct {
    Mesh423Rows mesh;
    u8 unknown_04EC[0x1640];
    POLY_GT4 polygon;
    u8 unknown_1B60[0x1F8];
    SVECTOR origin;
    s32 direction[3];
    u8 unknown_1D6C[0x20];
    s32 frame;
    u8 unknown_1D90[8];
    s32 step;
    u8 unknown_1D9C[0x1C];
    s32 size;
    u8 unknown_1DBC[0xC];
    s32 intensity;
    u8 unknown_1DCC[0x40];
    s32 phase;
} Mesh423State;

#endif
