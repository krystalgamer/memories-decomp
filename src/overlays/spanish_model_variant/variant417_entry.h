#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT417_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT417_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

/* One 0x44-byte command record of the table at gSpanishModelVariant417_aConfigs. */
typedef struct {
    u8 unknown_00[8];
    CVECTOR web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[5];
    u16 count, unknown_1A;
    s32 start;
    s32 expansion_start, expansion_end, fade_start, fade_end;
    u16 size, unknown_32;
    u8 unknown_34[4];
    s32 offset[3];
} Variant417EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
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
    u8 unknown_20AC[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count;
    s32 frame;
    u32 animation_frame;
    s32 step, fade;
    Variant417EntryConfig *config;
    u8 unknown_213C[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    s16 spiral_angle, field_2152, spiral_size, field_2156;
    s32 width, tube_progress, tube_angle;
    s16 field_2164, field_2166, progress, ribbon_width;
    s32 field_216C;
    s32 phase;
    s32 field_2174;
    u8 unknown_2178[8];
    s32 tint;
    s16 slot, command;
} Variant417EntryState;

extern GsIMAGE gSpanishModelVariant417_aImages[9];
extern Variant417EntryConfig gSpanishModelVariant417_aConfigs[];
void SpanishModelVariant417_DrawPhasedLines(u8 *context);
void SpanishModelVariant417_DrawTube(u8 *context);
void SpanishModelVariant441_DrawRainbowSheets(u8 *context);
void SpanishModelVariant417_DrawSpiral(u8 *context);
typedef char Variant417EntryConfigSize[(sizeof(Variant417EntryConfig) == 0x44) ? 1 : -1];
typedef char Variant417EntryStateSize[(sizeof(Variant417EntryState) == 0x2188) ? 1 : -1];
#endif
