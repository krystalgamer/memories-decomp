#ifndef FRENCH_MODEL_VARIANT461_ENTRY_H
#define FRENCH_MODEL_VARIANT461_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_effect_state.h"

typedef struct {
    CVECTOR outer, inner, web_color;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 first_count, paired;
    u32 draw_start, field_28, field_2C, phase3_end, matrix_end;
    u8 unknown_38[8];
} EntryConfig461;

typedef struct {
    u8 unknown_000[0x160];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_178, field_17C, count, field_184, state, length;
    u8 unknown_190[0x58];
    VECTOR position;
    u8 unknown_1F8[0x58];
} EntryRibbon461;

typedef struct {
    u8 unknown_00[0x68];
    s32 progress[2];
    CVECTOR outer[2], inner[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} EntryBand461;

typedef struct {
    ModelVariantSheet sheet;
    s32 field_98;
} EntryQuad461;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, state;
} EntrySmallRing461;

typedef struct {
    EntryRibbon461 ribbons[5];
    ModelVariantQuadRing ring;
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    EntryBand461 bands[1];
    EntryQuad461 quads[10];
    EntrySmallRing461 rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 unknown_2E34[0x34];
    POLY_FT4 flat[2], ring_packet;
    GsGLINE line;
    u8 unknown_2EF4[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR origin, path_origins[5], path_targets[5], paths[5], direction;
    DVECTOR screen_delta;
    VECTOR ring_positions[24], ring_directions[24], view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig461 *config;
    u32 field_3370;
    GsCOORDUNIT *parts[3];
    s32 angle, scale, wave, intensity, field_3390, field_3394;
    s16 field_3398, field_339A;
    s32 field_339C;
    s16 field_33A0, field_33A2;
    s32 field_33A4, field_33A8, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState461;

extern GsIMAGE gFrenchModelVariant461_aImages[9];
extern EntryConfig461 gFrenchModelVariant461_aConfigs[];
void FrenchModelVariant461_DrawRibbons(u8 *context);
void SpanishModelVariant461_DrawSheets(u8 *context);
void SpanishModelVariant461_DrawRing(u8 *context);

#endif
