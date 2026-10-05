#ifndef SPANISH_MODEL_VARIANT441_FRAMEBUFFER_RINGS_H
#define SPANISH_MODEL_VARIANT441_FRAMEBUFFER_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[3][17];
    u8 unknown_198[8];
    s32 progress;
    u8 unknown_1A4[4];
} FramebufferRings441;

typedef struct {
    u8 unknown_00[0x14D4];
    FramebufferRings441 groups[5];
    u8 unknown_1D1C[0x8B8];
    POLY_GT4 quad;
    u8 unknown_2608[0x94];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_26B0[0x20];
    s32 frame;
    u8 unknown_26D4[8];
    s32 step;
    u8 unknown_26E0[0x3C];
    s32 phase;
} FramebufferRings441State;

#endif
