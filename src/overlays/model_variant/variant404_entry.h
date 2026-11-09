#ifndef MEMORIES_MODEL_VARIANT404_ENTRY_H
#define MEMORIES_MODEL_VARIANT404_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 mode;
    u32 start;
    u8 unknown_24[0x10];
    s16 size;
    u8 unknown_36[6];
} Variant404EntryConfig;

typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantSpiral glows[12];
    u8 unknown_AB0[0x360];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1864[0x24];
    MATRIX matrix, matrix0, matrix1;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    s16 spiral_angle, field_1932, spiral_size, field_1936;
    Variant404EntryConfig *config;
    u32 field_193C;
    GsCOORDUNIT *parts[3];
    s16 field_194C, field_194E, band_scale, field_1952;
    s32 displacement, field_1958, ribbon_angle, phase, field_1964;
    u8 unknown_1968[8];
    s32 tint;
    s16 slot, command;
} Variant404EntryState;

extern GsIMAGE gModelVariant404_aImages[9];
extern Variant404EntryConfig gModelVariant404_aConfigs[];
void ModelVariant404_DrawRibbons(u8 *context);
void SpanishModelVariant421_DrawSpiral(u8 *context);
void ModelVariant404_DrawBands(u8 *context);
void ModelVariant404_DrawSheets(u8 *context);
void ModelVariant404_DrawWebs(u8 *context);
#endif
