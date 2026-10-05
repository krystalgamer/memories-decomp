#ifndef SPANISH_MODEL_VARIANT405_BANDS_H
#define SPANISH_MODEL_VARIANT405_BANDS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 size;
    s32 completed;
} Bands405Band;

typedef struct {
    u8 unknown_00[12];
    s32 count;
} Bands405Descriptor;

typedef struct {
    u8 unknown_0000[0x504];
    Bands405Band bands[3];
    u8 unknown_0864[0x25A4];
    POLY_GT4 polygons[16];
    u8 unknown_3148[0x88];
    s32 translation[3];
    u8 unknown_31DC[0x48];
    s32 step;
    u8 unknown_3228[8];
    Bands405Descriptor *G32 descriptor;
} Bands405State;

#endif
