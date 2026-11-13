#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

/* One 0x30-byte command record of the table at gSpanishModelVariant441_aConfigs. */
typedef struct {
    u8 unknown_00[8];
    CVECTOR web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[5];
    u16 count, unknown_1A;
    s32 start;
    s32 expansion_start, expansion_end, fade_start;
    s32 end;
} Variant441EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantScreenRing hoops[3];
    ModelVariantTubeView tubes[1];
    ModelVariantWebNarrow webs[3];
    ModelVariantShortRibbon ribbons[1];
    ModelVariantSheet sheets[2];
    u8 rays[0x360];
    ModelVariantTripleCurtain curtains[5];
    ModelVariantColorRing spokes[4];
    ModelVariantSpiral glows[12];
    ModelVariantPackets packets;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_2658[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    s32 frame_count;
    s32 frame;
    u32 animation_frame;
    s32 step, fade;
    Variant441EntryConfig *config;
    u8 unknown_26E8[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    s16 field_26FC, field_26FE, field_2700, field_2702;
    s32 width, tube_progress, tube_angle;
    s16 field_2710, field_2712, progress, ribbon_width;
    s32 field_2718;
    s32 phase;
    s32 field_2720;
    u8 unknown_2724[8];
    s32 tint;
    s16 slot, command;
} Variant441EntryState;

extern GsIMAGE gSpanishModelVariant441_aImages[9];
extern Variant441EntryConfig gSpanishModelVariant441_aConfigs[];
void SpanishModelVariant441_DrawTubularMesh(u8 *context);
void SpanishModelVariant441_DrawRainbowSheets(u8 *context);
void SpanishModelVariant441_DrawFramebufferRings(u8 *context);
void ModelVariant398_DrawRings(u8 *context);
typedef char Variant441EntryConfigSize[(sizeof(Variant441EntryConfig) == 0x30) ? 1 : -1];
typedef char Variant441EntryStateSize[(sizeof(Variant441EntryState) == 0x2734) ? 1 : -1];
#endif
