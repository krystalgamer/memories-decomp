#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT459_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT459_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x48-byte command record of the table at gSpanishModelVariant459_aConfigs. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u8 parts[6], unknown_16[2];
    u16 vertices[3], unknown_1E[3];
    u16 count, spiral;
    u16 limit;
    u8 unknown_2A[2];
    s32 curtains, mode;
    u32 start, end;
    u8 unknown_3C[0xC];
} Variant459EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantTripleCurtain curtains[5];
    ModelVariantCurtain rings[4];
    ModelVariantWebNarrow webs[3];
    ModelVariantSpiral glows[12];
    u8 rays[0x360];
    ModelVariantColorBand bands[6];
    ModelVariantSheet sheets[13];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 plain;
    POLY_GT4 ray;
    POLY_FT4 flat_textured[2];
    u8 unknown_2E80[0x4C];
    MATRIX matrices[6];
    VECTOR points[6];
    VECTOR directions[6];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 spiral_angle, field_3096, spiral_size, field_309A;
    Variant459EntryConfig *config;
    u8 unknown_30A0[4];
    GsCOORDUNIT *parts[6];
    s16 field_30BC, field_30BE;
    CVECTOR curtain_inner_color, curtain_outer_color;
    s32 curtain_angle;
    s16 scale, field_30CE;
    s32 progress, field_30D4, field_30D8;
    s32 phase;
    s32 field_30E0;
    u8 unknown_30E4[8];
    s32 tint;
    s16 slot, command;
} Variant459EntryState;

extern GsIMAGE gSpanishModelVariant459_aImages[9];
extern Variant459EntryConfig gSpanishModelVariant459_aConfigs[];
void SpanishModelVariant459_DrawCurtains(u8 *context);
void SpanishModelVariant459_DrawSpiral(u8 *context);
void SpanishModelVariant459_DrawTimedBands(u8 *context);
void SpanishModelVariant459_DrawPairedSheets(u8 *context);
void SpanishModelVariant459_DrawLines(u8 *context);
typedef char Variant459EntryConfigSize[(sizeof(Variant459EntryConfig) == 0x48) ? 1 : -1];
typedef char Variant459EntryStateSize[(sizeof(Variant459EntryState) == 0x30F4) ? 1 : -1];
#endif
