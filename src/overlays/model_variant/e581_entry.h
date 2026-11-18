#ifndef MEMORIES_MODEL_VARIANT398_ENTRY_H
#define MEMORIES_MODEL_VARIANT398_ENTRY_H
#include "../../types.h"
#include "../model_variant/h422_entry_pal.h"

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantScreenRing screen_rings[3];
    ModelVariantCurtain curtains[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_1974[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    ModelVariantWebConfig *config;
    u32 field_1A04;
    GsCOORDUNIT *parts[3];
    s16 field_1A14, field_1A16, band_scale, field_1A1A;
    s32 displacement, field_1A20;
    CVECTOR outer, inner;
    s32 curtain_angle, phase;
    u8 unknown_1A34[8];
    s32 tint;
    s16 slot, command;
} Variant398EntryState;

extern GsIMAGE gModelVariant398_aImages[9];
extern ModelVariantWebConfig gModelVariant398_aConfigs[];
void ModelVariant398_DrawBand(Variant398EntryState *context);
void ModelVariant398_DrawSheets(Variant398EntryState *context);
void ModelVariant398_DrawCurtains(Variant398EntryState *context);
#endif
