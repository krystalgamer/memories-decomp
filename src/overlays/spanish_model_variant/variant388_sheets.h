#ifndef SPANISH_MODEL_VARIANT388_SHEETS_H
#define SPANISH_MODEL_VARIANT388_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x0C];
    u16 count;
    u8 unknown_0E[2];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_18[4];
    u32 shrink_start;
    u32 shrink_end;
} Sheet388Timing;

typedef struct {
    u8 unknown_0000[0xF8];
    ModelVariantSheet sheets[2];
    u8 unknown_0228[0x14CC];
    POLY_GT4 polygon;
    u8 unknown_1728[0xF0];
    s32 origin[3];
    u8 unknown_1824[8];
    s32 direction[3];
    u8 unknown_1838[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1860[4];
    s32 step;
    u8 unknown_1868[4];
    Sheet388Timing *G32 timing;
    u8 unknown_1870[0x10];
    s32 index;
    u8 unknown_1884[0xC];
    s16 progress;
    u8 unknown_1892[6];
    s32 phase;
} Sheet388State;

#endif
