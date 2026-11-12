#ifndef MEMORIES_FRENCH_MODEL_VARIANT425_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT425_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../spanish_model_variant/variant425_mesh.h"
#include "variant423_funnel.h"
#include "../model_variant/variant425_streamers.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, ribbon_start, travel_start, travel_end, fade_start, fade_end;
} EntryConfig425;

/* A seventeen-point ribbon: its spine and a copy moved along the view, both
 * projected, and per point the screen angle, the projected width, the depth
 * and the width's screen offset. */
typedef struct {
    ModelVariantStripSpine spine;
    ModelVariantStripStyle style;
    u8 unknown_240[0x88];
    VECTOR position;
    ModelVariantStripOffsets offsets;
} EntryRibbon425;

typedef struct {
    ModelVariantMesh mesh;
    EntryRibbon425 ribbons[1];
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[2];
    Variant425Streamer streamers[1];
    Funnel423 funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1590[0x34];
    POLY_GT4 special;
    u8 unknown_15F8[0x34];
    ModelVariantPackets packets;
    POLY_FT4 flat[2];
    u8 unknown_1724[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig425 *config;
    u32 field_17A0;
    GsCOORDUNIT *part;
    s16 field_17A8, field_17AA;
    s32 width, field_17B0, size, field_17B8, field_17BC, field_17C0;
    s32 intensity, field_17C8, field_17CC, progress, travel;
    ModelVariantSpiralParams spiral;
    ModelVariantStreamerParams streamer;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, field_1800, field_1804, field_1808, phase;
    s32 field_1810, field_1814;
    u8 unknown_1818[8];
    s32 ambient;
    s16 slot, command;
} EntryState425;

extern GsIMAGE gFrenchModelVariant425_aImages[9];
extern EntryConfig425 gFrenchModelVariant425_aConfigs[];
void SpanishModelVariant425_DrawMesh(u8 *context);
void SpanishModelVariant425_DrawSpiral(u8 *context);
void FrenchModelVariant425_DrawRibbon(u8 *context);
void SpanishModelVariant425_DrawSheets(u8 *context);
void FrenchModelVariant423_DrawFunnel(u8 *context);

#endif
