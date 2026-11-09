#ifndef FRENCH175_TUBE_VIEW_H
#define FRENCH175_TUBE_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantTubeView tubes[1];
    u8 unknown_54C[0xED4];
    POLY_G4 quad;
    u8 unknown_1444[0x1B4];
    s32 origin[3];
    u8 unknown_1604[8];
    s32 direction[3];
    u8 unknown_1618[4];
    DVECTOR projected;
    s32 direction_b[3];
    u8 unknown_162C[4];
    ModelVariantClock clock;
    ModelVariantTubeTiming *timing;
    u8 unknown_1650[0xC];
    s32 progress;
    u8 unknown_1660[0xC];
    s32 width;
    s32 angle;
    u8 unknown_1674[8];
    s32 phase;
} Tube175State;

#endif
