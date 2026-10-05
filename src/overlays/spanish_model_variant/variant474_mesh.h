#ifndef SPANISH_MODEL_VARIANT474_MESH_H
#define SPANISH_MODEL_VARIANT474_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR row[9][17];
    u8 color[9][4];
} Mesh474Rows;

typedef struct {
    u8 unknown_0000[0x6DC];
    Mesh474Rows mesh;
    u8 unknown_0BC8[0xE88];
    POLY_G4 polygon;
    u8 unknown_1A74[0xE8];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_1B70[0x20];
    s32 frame;
    u8 unknown_1B94[8];
    s32 step;
    u8 unknown_1BA0[0x24];
    s32 size;
    u8 unknown_1BC8[0xC];
    s32 intensity;
    u8 unknown_1BD8[0x44];
    s32 phase;
} Mesh474State;

#endif
