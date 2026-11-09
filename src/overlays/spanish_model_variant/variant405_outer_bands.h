#ifndef SPANISH_MODEL_VARIANT405_OUTER_BANDS_H
#define SPANISH_MODEL_VARIANT405_OUTER_BANDS_H

#include "../../types.h"
#include "variant405_bands.h"

typedef struct {
    u8 unknown_00[0x64];
    s32 progress;
    u8 unknown_68[0xC];
} Outer405Primary;

typedef struct {
    Outer405Primary primary[3];
    u8 unknown_015C[0x708];
    Bands405Band bands[3];
    u8 unknown_0BC4[0x2244];
    POLY_GT4 polygons[16];
    u8 unknown_3148[0x94];
    SVECTOR origin;
    u8 unknown_31E4[0x2C];
    ModelVariantClock clock;
    u8 unknown_322C[4];
    Bands405Descriptor *descriptor;
    u8 unknown_3234[0x20];
    s32 phase;
} Outer405State;

#endif
