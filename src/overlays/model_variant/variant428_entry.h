#ifndef MEMORIES_MODEL_VARIANT428_ENTRY_H
#define MEMORIES_MODEL_VARIANT428_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant428_spiral.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    s32 field_20;
    s32 field_24;
    s32 field_28;
    s32 field_2C;
    u32 spiral_start;
} Variant428EntryConfig;

typedef struct {
    ModelVariantShadedRay arms[12];
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1504[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant428EntryConfig *config;
    u32 field_1594;
    GsCOORDUNIT *parts[3];
    s16 field_15A4, field_15A6, spiral_size, field_15AA, field_15AC, field_15AE;
    s16 scale, field_15B2;
    s32 progress, field_15B8;
    ModelVariantEntryTail tail;
} Variant428EntryState;

extern GsIMAGE gModelVariant428_aImages[9];
extern Variant428EntryConfig gModelVariant428_aConfigs[];
void ModelVariant428_DrawBands(u8 *context);
void ModelVariant428_DrawSheets(u8 *context);
void ModelVariant428_DrawWebs(u8 *context);
void ModelVariant428_DrawSpiral(u8 *context);
#endif
