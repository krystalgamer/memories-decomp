#ifndef SPANISH_MODEL_VARIANT443_RINGS_H
#define SPANISH_MODEL_VARIANT443_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[3][4];
    u8 unknown_60[8];
    s32 progress;
    u8 unknown_6C[4];
} FramebufferRings443;

typedef struct {
    u8 unknown_00[0xA50];
    FramebufferRings443 groups[5];
    u8 unknown_C80[0x27B8];
    POLY_GT4 quad;
    u8 unknown_346C[0x144];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_35C4[0x5C];
    u32 frame;
    u8 unknown_3624[8];
    s32 step;
    u8 unknown_3630[0x2C];
    s32 spin;
    u8 unknown_3660[0x38];
    s32 phase;
} FramebufferRings443State;

#endif
