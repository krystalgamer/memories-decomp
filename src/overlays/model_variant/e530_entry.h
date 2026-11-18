#ifndef MEMORIES_MODEL_VARIANT418_ENTRY_H
#define MEMORIES_MODEL_VARIANT418_ENTRY_H
#include "../../types.h"
#include "../model_variant/e503_h320_entry_pal.h"
#include "e530_spiral.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13;
    u16 vertex;
    u8 unknown_16[0x0A];
    u16 count;
    u8 unknown_22[2];
    s32 mode;
    u32 start;
    u32 burst_start;
    u32 burst_end;
    u32 shrink_start;
    u32 shrink_end;
    u32 end;
    u8 unknown_40[4];
} Variant418EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWeb webs[3];
    ModelVariantFlaggedSpiralArm arms[12];
    ModelVariantRibbon ribbons[8];
    ModelVariantColorBandPadded bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1AD0[0x10];
    GsGLINE line;
    MATRIX matrix;
    VECTOR vertex_position;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantSpiralParams spiral;
    Variant418EntryConfig *config;
    u32 field_1B78;
    GsCOORDUNIT *parts[3];
    s16 index, field_1B8A, band_scale, field_1B8E;
    s32 displacement, quad_angle, ribbon_angle;
    s32 phase, field_1BA0;
    u8 unknown_1BA4[8];
    s32 tint;
    s16 slot, command;
} Variant418EntryState;

extern GsIMAGE gModelVariant418_aImages[9];
extern Variant418EntryConfig gModelVariant418_aConfigs[];
void ModelVariant418_DrawSpiral(Variant418EntryState *context);
void ModelVariant418_DrawSheet(Variant418EntryState *context);
void ModelVariant418_DrawWebs(Variant418EntryState *context);
#endif
