#ifndef FRENCH_MODEL_VARIANT383_RAYS_H
#define FRENCH_MODEL_VARIANT383_RAYS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    u8 unknown_0000[0xB74];
    ModelVariantShadedRay rays[16];
    u8 unknown_1334[0x58];
    POLY_GT4 quad;
    u8 unknown_13C0[0x124];
    s32 origin[3];
    u8 unknown_14F0[8];
    s32 delta[3];
    u8 unknown_1504[8];
    s32 view_direction[3];
    u8 unknown_1518[0xC];
    s32 flags;
    u8 unknown_1528[8];
    s32 step;
    u8 unknown_1534[0x1C];
    s32 scale;
    s32 brightness;
    u8 unknown_1558[0xC];
    s16 angle_a;
    s16 angle_b;
    u8 unknown_1568[4];
    s16 radius;
    u8 unknown_156E[0xA];
    s16 factor;
    u8 unknown_157A[6];
    s32 phase;
} Family383RayView;

void FrenchModelVariant383_DrawRays(u8 *context);
#endif
