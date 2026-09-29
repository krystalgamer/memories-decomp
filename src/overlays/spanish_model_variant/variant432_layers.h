#ifndef MEMORIES_MODEL_VARIANT432_LAYERS_H
#define MEMORIES_MODEL_VARIANT432_LAYERS_H
#include "../../types.h"
#include "variant432_bands.h"

typedef struct {
    u8 unknown_00[0x64];
    s32 value;
    u8 unknown_68[12];
} ModelVariant432Source;

typedef struct {
    VECTOR position;
    u8 unknown_10[16];
} ModelVariant432Position;

typedef struct {
    ModelVariant432Source sources[3];
    u8 unknown_15C[0x130];
    ModelVariant432Band bands[3];
    u8 unknown_5EC[0x25A4];
    POLY_GT4 quads[16];
    u8 unknown_2ED0[0xC4];
    ModelVariant432Position positions[5];
} ModelVariant432LayersState;

#endif
