#ifndef SPANISH_MODEL_VARIANT438_LINES_H
#define SPANISH_MODEL_VARIANT438_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0x960];
    ModelVariantLineGroup groups[3];
    u8 unknown_0E40[0x13F0];
    GsGLINE line;
    u8 unknown_2244[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_226C[4];
    s32 direction_a[3];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_228C[0x10];
    u32 time;
    u8 unknown_22A0[4];
    u32 step;
    u8 unknown_22A8[0xC];
    ModelVariantGrowTiming *timing;
    u8 unknown_22B8[0x30];
    s32 phase;
} Lines438State;

#endif
