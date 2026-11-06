#ifndef MEMORIES_FRENCH_MODEL_VARIANT439_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT439_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
} Variant439EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} Variant439EntryBand;

typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR inner, outer;
    s32 scale, count;
} Variant439EntryScreenRing;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant439EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    ModelVariantFan fans[1];
    Variant439EntryScreenRing screen_rings[3];
    ModelVariantCurtain curtains[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_18C0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant439EntryConfig *config;
    u32 field_1950;
    GsCOORDUNIT *parts[3];
    s16 field_1960, field_1962, field_1964, field_1966;
    s32 field_1968, field_196C;
    CVECTOR outer, inner;
    s32 field_1978, phase;
    u8 unknown_1980[8];
    s32 tint;
    s16 slot, command;
} Variant439EntryState;

extern GsIMAGE gFrenchModelVariant439_aImages[9];
extern Variant439EntryConfig gFrenchModelVariant439_aConfigs[];
void ModelVariant422_DrawBands(u8 *context);
void ModelVariant422_DrawSheets(u8 *context);
void ModelVariant422_DrawCurtains(u8 *context);
#endif
