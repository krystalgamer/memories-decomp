#ifndef SPANISH_MODEL_VARIANT385_PULSE_H
#define SPANISH_MODEL_VARIANT385_PULSE_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x56C];
    ModelVariantPulseGroup groups[2];
    u8 unknown_68C[0x7D4];
    POLY_GT4 quad;
    u8 unknown_E94[0x118];
    s32 origin[3];
    u8 unknown_FB8[8];
    s32 direction[3];
    u8 unknown_FCC[0x20];
    s32 frame;
    u32 time;
    u8 unknown_FF4[4];
    s32 step;
    u8 unknown_FFC[4];
    ModelVariantPulseTiming *timing;
    u8 unknown_1004[0x10];
    s32 index;
    u8 unknown_1018[0x12];
    s16 progress;
    u8 unknown_102C[4];
    s32 phase;
} Pulse385State;

#endif
