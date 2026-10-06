#ifndef SPANISH_MODEL_VARIANT436_SHEETS_H
#define SPANISH_MODEL_VARIANT436_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    CVECTOR outer;
    CVECTOR inner;
} Sheet436;

typedef struct {
    u8 unknown_00[0x10];
    s32 grow_start;
    s32 grow_end;
    s32 phase2_end;
    s32 phase3_end;
    s32 travel_end;
} Timing436;

typedef struct {
    u8 unknown_00[0x800];
    Sheet436 sheets[1];
    u8 unknown_888[0xB68];
    POLY_GT4 packets[2];
    u8 unknown_1458[0xD0];
    s32 origin[3];
    u8 unknown_1534[0x5C];
    s32 progress;
    s32 phase2_progress;
    s32 size;
    s32 phase3_progress;
    u8 unknown_15A0[0x10];
    s32 path[3];
    u8 unknown_15BC[8];
    s32 direction[3];
    u8 unknown_15D0[0xC];
    s32 frame;
    s32 time;
    u8 unknown_15E4[4];
    s32 step;
    u8 unknown_15EC[8];
    Timing436 *G32 timing;
    u8 unknown_15F8[0x44];
    s32 phase;
} Sheet436State;

#endif
