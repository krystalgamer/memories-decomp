#ifndef SPANISH_MODEL_VARIANT427_CURTAINS_H
#define SPANISH_MODEL_VARIANT427_CURTAINS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_000[0x4EC];
    s32 scale;
} Curtain427Primary;

typedef struct {
    Curtain427Primary primary[3];
    u8 unknown_0ED0[0x1BA8];
    ModelVariantCurtain curtains[9];
    u8 unknown_3450[0x34];
    POLY_GT4 polygon;
    u8 unknown_34B8[0x1D8];
    VECTOR origins[3];
    u8 unknown_36C0[0x88];
    s32 step;
    u8 unknown_374C[0x24];
    s32 size;
    u8 unknown_3774[0xC];
    s32 intensity;
    u8 unknown_3784[0x24];
    u8 inner[4];
    u8 outer[4];
    s32 angle;
    u8 unknown_37B4[0x10];
    s32 phase;
} Curtain427State;

#endif
