#ifndef MEMORIES_MODEL_VARIANT428_ENTRY_H
#define MEMORIES_MODEL_VARIANT428_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant428_spiral.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
    u32 spiral_start;
} Variant428EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} Variant428EntryBand;

typedef struct {
    Variant428SpiralArm arms[12];
    ModelVariantWebNarrow webs[3];
    Variant428EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1504[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant428EntryConfig *config;
    u32 field_1594;
    GsCOORDUNIT *parts[3];
    s16 field_15A4, field_15A6, field_15A8, field_15AA, field_15AC, field_15AE;
    s16 field_15B0, field_15B2;
    s32 field_15B4, field_15B8;
    s32 phase;
    u8 unknown_15C0[8];
    s32 tint;
    s16 slot, command;
} Variant428EntryState;

extern GsIMAGE gModelVariant428_aImages[9];
extern Variant428EntryConfig gModelVariant428_aConfigs[];
void ModelVariant428_DrawBands(u8 *context);
void ModelVariant428_DrawSheets(u8 *context);
void ModelVariant428_DrawWebs(u8 *context);
void ModelVariant428_DrawSpiral(u8 *context);
#endif
