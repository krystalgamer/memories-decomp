#ifndef MEMORIES_MODEL_VARIANT404_ENTRY_H
#define MEMORIES_MODEL_VARIANT404_ENTRY_H
#include "../../types.h"
#include "../model_variant/h320_entry_pal.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[5];
    u16 count;
    u8 unknown_1A[2];
    s32 mode;
    u32 start;
    u32 burst_start;
    u32 burst_end;
    u32 shrink_start;
    u32 shrink_end;
    s16 size;
    u8 unknown_36[0x2];
    s32 radius;
} Variant404EntryConfig;

typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantShadedRay glows[12];
    ModelVariantRibbonShort ribbons[8];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1864[0x10];
    GsGLINE line;
    MATRIX matrix, matrix0, matrix1;
    ModelVariantAim aim;
    ModelVariantClock clock;
    s16 spiral_angle, field_1932, spiral_size, spiral_shrink;
    Variant404EntryConfig *config;
    u32 field_193C;
    GsCOORDUNIT *parts[3];
    s16 field_194C, field_194E, band_scale, field_1952;
    s32 displacement, quad_angle, ribbon_angle, phase, spiral_state;
    u8 unknown_1968[8];
    s32 tint;
    s16 slot, command;
} Variant404EntryState;

extern GsIMAGE gModelVariant404_aImages[9];
extern Variant404EntryConfig gModelVariant404_aConfigs[];
void ModelVariant404_DrawRibbons(Variant404EntryState *context);
void SpanishModelVariant421_DrawSpiral(Variant404EntryState *context);
void ModelVariant404_DrawBands(Variant404EntryState *context);
void ModelVariant404_DrawSheets(Variant404EntryState *context);
void ModelVariant404_DrawWebs(Variant404EntryState *context);
#endif
