#ifndef MEMORIES_MODEL_VARIANT428_ENTRY_H
#define MEMORIES_MODEL_VARIANT428_ENTRY_H
#include "../../types.h"
#include "../model_variant/h320_entry_pal.h"
#include "h428_spiral.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 burst_start;
    u32 burst_end;
    u32 shrink_start;
    u32 shrink_end;
    u32 spiral_start;
} Variant428EntryConfig;

typedef struct {
    ModelVariantShadedRay arms[12];
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1504[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant428EntryConfig *config;
    u32 field_1594;
    GsCOORDUNIT *parts[3];
    s16 spiral_angle, field_15A6, spiral_size, field_15AA, field_15AC, field_15AE;
    s16 scale, field_15B2;
    s32 progress, quad_angle;
    ModelVariantEntryTail tail;
} Variant428EntryState;

extern GsIMAGE gModelVariant428_aImages[9];
extern Variant428EntryConfig gModelVariant428_aConfigs[];
void ModelVariant428_DrawBands(Variant428EntryState *context);
void ModelVariant428_DrawSheets(Variant428EntryState *context);
void ModelVariant428_DrawWebs(Variant428EntryState *context);
void ModelVariant428_DrawSpiral(Variant428EntryState *context);
#endif
