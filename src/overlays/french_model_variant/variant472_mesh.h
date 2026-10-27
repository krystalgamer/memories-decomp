#ifndef FRENCH472_MESH_H
#define FRENCH472_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x20];
    u32 start;
    u32 end;
    u8 unknown_28[4];
    u32 fade_start;
    u32 fade_end;
} Mesh719Timing;

typedef struct {
    SVECTOR front[7][17];
    SVECTOR back[7][17];
    u8 unknown_770[0x1DC];
    s32 progress;
    u8 unknown_950[4];
} Mesh719;

typedef struct {
    u8 unknown_0000[0x15A0];
    Mesh719 meshes[1];
    u8 unknown_1EF4[0xD08];
    POLY_F4 mask;
    u8 unknown_2C14[0x1A0];
    POLY_GT4 quad;
    u8 unknown_2DE8[0xBC];
    s32 position[3];
    u8 unknown_2EB0[0xC];
    s32 direction[3];
    u8 unknown_2EC8[0x24];
    u32 time;
    u8 unknown_2EF0[4];
    s32 step;
    u8 unknown_2EF8[4];
    Mesh719Timing *G32 timing;
    u8 unknown_2F00[0x2C];
    s32 angle;
    u8 unknown_2F30[0x10];
    s32 phase;
} Mesh719State;

#endif
