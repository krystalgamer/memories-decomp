#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#include "../../types.h"
#include "../../overlays/model_variant/h320_entry_pal.h"
#include "../../game/model_slot_getters.h"
#include "e581_entry.h"

/* One 0x30-byte command record of the table at gSpanishModelVariant441_aConfigs. */
typedef struct {
    u8 unknown_00[8];
    CVECTOR web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[5];
    u16 count, unknown_1A;
    s32 start;
    s32 expansion_start, expansion_end, fade_start, fade_end;
} Variant441EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantScreenRing screen_rings[3];
    ModelVariantTube tubes[1];
    ModelVariantWebNarrow webs[3];
    ModelVariantShortRibbon ribbons[1];
    ModelVariantSheet sheets[2];
    ModelVariantOffsetRay rays[8];
    ModelVariantTripleCurtain curtains[5];
    ModelVariantColorRing spokes[4];
    ModelVariantShadedRay glows[12];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_2658[0x10];
    /* The lines helper's packet. */
    GsGLINE line;
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
    s16 spiral_angle, field_26FE, spiral_size, spiral_shrink;
    s32 width, tube_progress, tube_angle;
    s16 field_2710, field_2712, progress, ribbon_width;
    s32 ray_angle;
    s32 phase;
    s32 field_2720;
    u8 unknown_2724[8];
    s32 tint;
    s16 slot, command;
} Variant441EntryState;

extern GsIMAGE gSpanishModelVariant441_aImages[9];
extern Variant441EntryConfig gSpanishModelVariant441_aConfigs[];
void SpanishModelVariant441_DrawTubularMesh(Variant441EntryState *context);
void SpanishModelVariant441_DrawRibbon(Variant441EntryState *context);
void SpanishModelVariant441_DrawRainbowSheets(Variant441EntryState *context);
void SpanishModelVariant441_DrawFramebufferRings(Variant441EntryState *context);
void ModelVariant398_DrawRings(Variant441EntryState *context);
typedef char Variant441EntryConfigSize[(sizeof(Variant441EntryConfig) == 0x30) ? 1 : -1];
typedef char Variant441EntryStateSize[(sizeof(Variant441EntryState) == 0x2734) ? 1 : -1];
#endif
