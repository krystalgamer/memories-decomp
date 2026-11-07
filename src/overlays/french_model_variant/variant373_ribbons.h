#ifndef FRENCH_MODEL_VARIANT373_RIBBONS_H
#define FRENCH_MODEL_VARIANT373_RIBBONS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    u8 unknown_0000[0xFD0];
    ModelVariantRibbonColors colors;
    ModelVariantRibbonSpan ribbons[8];
    u8 unknown_13B8[0xA00];
    POLY_G3 triangle;
    u8 unknown_1DD4[0x5F0];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[8];
    s32 view_direction[3];
    u8 unknown_23F8[0xC];
    s32 flags;
    u8 unknown_2408[8];
    s32 step;
    u8 unknown_2414[4];
    ModelVariantCountConfig *config;
    u8 unknown_241C[0x10];
    s32 index;
    u8 unknown_2430[0x14];
    s32 scale;
    u8 unknown_2448[0x2C];
    s16 factor;
    u8 unknown_2476[2];
    s32 angle;
    u8 unknown_247C[8];
    s32 phase;
    u8 unknown_2488[0x14];
    s16 mode;
} Family373RibbonView;

void FrenchModelVariant373_DrawRibbons(u8 *context);
#endif
