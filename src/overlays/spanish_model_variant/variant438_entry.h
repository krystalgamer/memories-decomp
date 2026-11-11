#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

/* One 0x48-byte command record of the table at gSpanishModelVariant438_aConfigs. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[8];
    u32 fade_start, fade_end;
    s32 offset[3];
    u16 sheet_size, unknown_3E;
    s32 radius;
    s32 size;
} Variant438EntryConfig;

/* A six-point fan, five rays, four corners and two colours. */
typedef struct {
    SVECTOR points[6];
    SVECTOR rays[5];
    SVECTOR corners[4];
    CVECTOR color_a, color_b;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} Variant438EntryStar;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantTripleCurtain curtains[5];
    ModelVariantCurtain rings[1];
    ModelVariantWebNarrow webs[3];
    ModelVariantSpiral glows[12];
    u8 rays[0x360];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing spokes[6];
    ModelVariantColorRing petals[4];
    Variant438EntryStar stars[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 ray;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_21F8[0x4C];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantSpiralParams spiral;
    Variant438EntryConfig *config;
    u8 unknown_22B8[4];
    GsCOORDUNIT *parts[3];
    s16 field_22C8, field_22CA;
    CVECTOR color_22CC, color_22D0;
    s32 ring_angle;
    s16 scale, field_22DA;
    s32 progress, field_22E0, field_22E4;
    s32 phase;
    s32 field_22EC;
    u8 unknown_22F0[8];
    s32 tint;
    s16 slot, command;
} Variant438EntryState;

extern GsIMAGE gSpanishModelVariant438_aImages[9];
extern Variant438EntryConfig gSpanishModelVariant438_aConfigs[];
void SpanishModelVariant438_DrawWarpRings(u8 *context);
void SpanishModelVariant438_DrawRing(u8 *context);
void SpanishModelVariant438_DrawSpiral(u8 *context);
void SpanishModelVariant438_DrawAdvancingBand(u8 *context);
void SpanishModelVariant438_DrawTargetClampedSheets(u8 *context);
void SpanishModelVariant438_DrawLines(u8 *context);
typedef char Variant438EntryConfigSize[(sizeof(Variant438EntryConfig) == 0x48) ? 1 : -1];
typedef char Variant438EntryStateSize[(sizeof(Variant438EntryState) == 0x2300) ? 1 : -1];
#endif
