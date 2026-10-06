#ifndef SPANISH_MODEL_VARIANT437_SHEET_H
#define SPANISH_MODEL_VARIANT437_SHEET_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
} Sheet437;

typedef struct {
    u8 unknown_00[0x10];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_18[4];
    u32 move_start;
    u32 move_end;
} Timing437;

typedef struct {
    u8 unknown_00[0x800];
    Sheet437 sheets[1];
    u8 unknown_0888[0xB68];
    POLY_GT4 polygons[2];
    u8 unknown_1458[0xD0];
    s32 origin[3];
    u8 unknown_1534[0x5C];
    s32 progress;
    u8 unknown_1594[4];
    s32 size;
    u8 unknown_159C[0x14];
    s32 path[3];
    u8 unknown_15BC[8];
    s32 direction[3];
    u8 unknown_15D0[0xC];
    u32 frame;
    u32 time;
    u8 unknown_15E4[4];
    s32 step;
    u8 unknown_15EC[8];
    Timing437 *G32 timing;
    u8 unknown_15F8[0x44];
    s32 phase;
} Sheet437State;

#endif
