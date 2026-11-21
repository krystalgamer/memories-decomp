#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#include "../../types.h"
#include "../../overlays/model_variant/e503_h320_entry_pal.h"
#include "../../game/model_slot_getters.h"

/* One 0x48-byte command record of the table at gSpanishModelVariant438_aConfigs. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[5];
    /* The rays helper redraws its rays while index + 1 == count. */
    u16 count;
    u8 unknown_1A[2];
    u32 start;
    u32 grow_end;
    u32 band_grow_end;
    u32 fade_start, fade_end;
    s32 offset[3];
    u16 sheet_size, unknown_3E;
    s32 radius;
    s32 size;
} SpanishVariant438EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantTripleCurtain curtains[5];
    ModelVariantCurtain rings[1];
    ModelVariantWebNarrow webs[3];
    ModelVariantShadedRay glows[12];
    ModelVariantOffsetRay rays[8];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing spokes[6];
    ModelVariantColorRing petals[4];
    ModelVariantStarShape stars[1];
    ModelVariantPackets packets;
    POLY_GT4 ray;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_21F8[0x38];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantSpiralParams spiral;
    SpanishVariant438EntryConfig *config;
    u8 unknown_22B8[4];
    GsCOORDUNIT *parts[3];
    s16 index, field_22CA;
    CVECTOR color_22CC, color_22D0;
    s32 ring_angle;
    s16 scale, field_22DA;
    s32 progress, field_22E0, ray_angle;
    s32 phase;
    s32 spiral_state;
    u8 unknown_22F0[8];
    s32 tint;
    s16 slot, command;
} SpanishVariant438EntryState;

extern GsIMAGE gSpanishModelVariant438_aImages[9];
extern SpanishVariant438EntryConfig gSpanishModelVariant438_aConfigs[];
void SpanishModelVariant438_DrawWarpRings(SpanishVariant438EntryState *context);
void SpanishModelVariant438_DrawRing(SpanishVariant438EntryState *context);
void SpanishModelVariant438_DrawSpiral(SpanishVariant438EntryState *context);
void SpanishModelVariant438_DrawAdvancingBand(SpanishVariant438EntryState *context);
void SpanishModelVariant438_DrawTargetClampedSheets(SpanishVariant438EntryState *context);
void SpanishModelVariant438_DrawLines(SpanishVariant438EntryState *context);
typedef char SpanishVariant438EntryConfigSize[(sizeof(SpanishVariant438EntryConfig) == 0x48) ? 1 : -1];
typedef char SpanishVariant438EntryStateSize[(sizeof(SpanishVariant438EntryState) == 0x2300) ? 1 : -1];
#endif
