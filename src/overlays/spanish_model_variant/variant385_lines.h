#ifndef SPANISH_MODEL_VARIANT385_LINES_H
#define SPANISH_MODEL_VARIANT385_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantLineGroupRgb groups[3];
    u8 unknown_4E0[0xAA4];
    GsGLINE line;
    u8 unknown_F98[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_FC0[4];
    s32 direction_a[2];
    u8 unknown_FCC[4];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_FE0[4];
    ModelVariantClock clock;
    ModelVariantLineTiming *timing;
    u8 unknown_1004[0x2C];
    s32 phase;
} State385;

#endif
