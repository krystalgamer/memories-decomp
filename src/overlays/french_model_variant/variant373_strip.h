#ifndef FRENCH_MODEL_VARIANT373_STRIP_H
#define FRENCH_MODEL_VARIANT373_STRIP_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    u8 unknown_0000[0xF64];
    ModelVariantStrip strip;
    u8 unknown_0FD0[0xE28];
    POLY_GT4 quad;
    u8 unknown_1E2C[0x598];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[4];
    s16 angle_delta[2];
    u8 unknown_23EC[0x24];
    s32 step;
    u8 unknown_2414[4];
    ModelVariantCountConfig *config;
    u8 unknown_241C[0x10];
    s32 index;
    u8 unknown_2430[0x44];
    s16 factor;
    s16 width;
    u8 unknown_2478[0xC];
    s32 phase;
} Family373StripView;

void FrenchModelVariant373_DrawStrip(u8 *context);
#endif
