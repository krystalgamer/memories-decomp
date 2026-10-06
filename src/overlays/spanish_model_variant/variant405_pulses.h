#ifndef SPANISH_MODEL_VARIANT405_PULSES_H
#define SPANISH_MODEL_VARIANT405_PULSES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x62];
    s16 started;
    s32 progress;
    u8 unknown_68[0xC];
} Pulse405Primary;

typedef struct {
    Pulse405Primary primary[3];
    ModelVariantSheetSet groups[6];
    u8 unknown_504[0x28D0];
    POLY_GT4 quad;
    u8 unknown_2E08[0x3C8];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_31E4[0x34];
    u32 frame;
    u32 time;
    u8 unknown_3220[4];
    u32 step;
    u8 unknown_3228[4];
    s32 count;
} Pulse405State;

#endif
