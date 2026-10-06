#ifndef SPANISH_MODEL_VARIANT444_SHEETS_H
#define SPANISH_MODEL_VARIANT444_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0xC];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_14[4];
    u32 fade_start;
    u32 fade_end;
} Timing444;

typedef struct {
    u8 unknown_00[0x54C];
    ModelVariantSheet sheets[2];
    u8 unknown_67C[0xDFC];
    POLY_GT4 quad;
    u8 unknown_14AC[0x14C];
    s32 origin[3];
    u8 unknown_1604[8];
    s32 path[3];
    u8 unknown_1618[0x20];
    u32 frame;
    u32 time;
    u8 unknown_1640[4];
    u32 step;
    u8 unknown_1648[4];
    Timing444 *G32 timing;
    u8 unknown_1650[0xC];
    s32 progress;
    u8 unknown_1660[0xC];
    s32 fade;
    u8 unknown_1670[0xC];
    s32 phase;
} Sheet444State;

#endif
