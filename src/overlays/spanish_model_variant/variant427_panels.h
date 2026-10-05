#ifndef SPANISH_MODEL_VARIANT427_PANELS_H
#define SPANISH_MODEL_VARIANT427_PANELS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_000[0x240];
    s32 progress;
    u8 unknown_244[0x164];
} Panel427Primary;

typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[4];
    u32 shrink_start;
    u32 shrink_end;
} Panel427Descriptor;

typedef struct {
    u8 unknown_0000[0xED0];
    Panel427Primary primary[3];
    u8 unknown_19C8[0x630];
    ModelVariantSheet sheets[6];
    u8 unknown_2388[0x120C];
    POLY_GT4 polygon;
    u8 unknown_35C8[0x60];
    MATRIX matrices[3];
    u8 unknown_3688[0x68];
    VECTOR directions[3];
    u8 unknown_3720[0x1C];
    s32 frame;
    u32 time;
    u8 unknown_3744[4];
    s32 step;
    u8 unknown_374C[4];
    Panel427Descriptor *G32 descriptor;
    u8 unknown_3754[0x70];
    s32 phase;
} Panel427State;

#endif
