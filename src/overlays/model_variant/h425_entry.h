#ifndef MEMORIES_MODEL_VARIANT425_ENTRY_H
#define MEMORIES_MODEL_VARIANT425_ENTRY_H
#include "../../types.h"
#include "../model_variant/h320_entry_pal.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[5];
    u16 count;
    u8 unknown_1A[2];
    u32 start;
    u32 burst_start;
    u32 burst_end;
    u32 shrink_start;
    u32 shrink_end;
    u32 fade_end;
    u8 unknown_34[0x4];
} Variant425EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebSmall webs[3];
    ModelVariantShadedRay arms[16];
    ModelVariantRay rays[16];
    ModelVariantRibbonShort ribbons[8];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_265C[0x10];
    GsGLINE line;
    MATRIX matrix;
    SVECTOR target;
    VECTOR orbit;
    VECTOR direction;
    VECTOR offset;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    s16 spiral_angle, field_270A, spiral_size, field_270E;
    s32 field_2710;
    Variant425EntryConfig *config;
    u32 field_2718;
    GsCOORDUNIT *parts[3];
    s16 index, field_272A, band_scale, field_272E;
    s32 displacement, sweep, spiral_shrink, ray_scale, quad_angle, ribbon_angle;
    s32 phase, field_274C;
    u8 unknown_2750[8];
    s32 tint;
    s16 slot, command;
} Variant425EntryState;

extern GsIMAGE gModelVariant425_aImages[9];
extern Variant425EntryConfig gModelVariant425_aConfigs[];
void ModelVariant425_DrawSheet(Variant425EntryState *context);
void ModelVariant425_DrawSpiral(Variant425EntryState *context);
void ModelVariant425_DrawRays(Variant425EntryState *context);
void ModelVariant425_DrawWebs(Variant425EntryState *context);
#endif
