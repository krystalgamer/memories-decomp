#ifndef MEMORIES_MODEL_VARIANT398_ENTRY_H
#define MEMORIES_MODEL_VARIANT398_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant439_entry.h"

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantScreenRing screen_rings[3];
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
    ModelVariantWebConfig *config;
    u32 field_1950;
    GsCOORDUNIT *parts[3];
    s16 field_1960, field_1962, field_1964, field_1966;
    s32 field_1968, field_196C;
    CVECTOR outer, inner;
    s32 field_1978, phase;
    u8 unknown_1980[8];
    s32 tint;
    s16 slot, command;
} Variant398EntryState;

/* Header-441 context for the same rings: the rings lead it, and the packet,
 * view and timing fields sit 0xCDC-0xD18 bytes higher than in header 398. */
typedef struct {
    ModelVariantScreenRing screen_rings[3];
    u8 unknown_5AC[0x25D4 - 0x5AC];
    POLY_GT4 extra;
    u8 unknown_2608[0x267C - 0x2608];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    s32 frame_count;
    u32 frame, animation_frame;
    s32 step;
    u8 unknown_26E0[0x271C - 0x26E0];
    s32 phase;
} Variant398Header441RingsState;

extern GsIMAGE gModelVariant398_aImages[9];
extern ModelVariantWebConfig gModelVariant398_aConfigs[];
void ModelVariant398_DrawRings(u8 *context);
void ModelVariant398_DrawBand(u8 *context);
void ModelVariant398_DrawSheets(u8 *context);
void ModelVariant398_DrawCurtains(u8 *context);
#endif
