#ifndef SPANISH_MODEL_VARIANT441_RAYS_H
#define SPANISH_MODEL_VARIANT441_RAYS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1044];
    ModelVariantPulse pulse;
    u8 unknown_10D0[0xA4];
    ModelVariantOffsetRay rays[8];
    u8 unknown_14D4[0x1058];
    POLY_G3 triangle;
    u8 unknown_2548[0x148];
    s32 origin[3];
    u8 unknown_269C[8];
    s32 direction[3];
    u8 unknown_26B0[8];
    s32 direction_b[3];
    u8 unknown_26C4[0xC];
    s32 frame;
    u8 unknown_26D4[8];
    s32 step;
    u8 unknown_26E0[4];
    ModelVariantRayTiming *timing;
    u8 unknown_26E8[0x10];
    s32 index;
    u8 unknown_26FC[0x18];
    s16 progress;
    u8 unknown_2716[2];
    s32 angle;
    u8 unknown_271C[0x14];
    s16 mirror;
} Rays441State;

#endif
