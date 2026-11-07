#ifndef FRENCH_MODEL_VARIANT473_ENTRY_H
#define FRENCH_MODEL_VARIANT473_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "../french_model_variant/variant385_grid.h"
#include "../model_variant/variant458_streamers.h"
#include "../spanish_model_variant/variant473_funnels.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start;
    u32 grow_end;
    u8 unknown_20[4];
    u32 fade_begin, fade_end;
} EntryConfig473;

typedef struct {
    u8 unknown_000[0x120];
    CVECTOR inner[9], outer[9];
    VECTOR scale;
    s16 field_178, field_17A;
    s32 field_17C, progress, field_184, field_188;
    u8 unknown_18C[0x48];
    VECTOR position;
    u8 unknown_1E4[0x48];
} EntryRibbon473;

typedef struct {
    ModelVariantGridSheet meshes[5];
    EntryRibbon473 ribbons[5];
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[6];
    Variant458Streamer streamers[2];
    ModelVariantFunnel funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_395C[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2], additional[2];
    u8 unknown_3AD8[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR origins[5], direction, ribbon_directions[5];
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig473 *config;
    u32 field_3BF4;
    GsCOORDUNIT *part;
    s16 field_3BFC, field_3BFE;
    s32 width, field_3C04, size, field_3C0C, field_3C10, field_3C14;
    s32 intensity, field_3C1C, field_3C20, progress;
    s16 spiral_angle, field_3C2A, spiral_size, field_3C2E;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, field_3C50, field_3C54, field_3C58, phase;
    s32 field_3C60, field_3C64;
    u8 unknown_3C68[8];
    s32 ambient;
    s16 slot, command;
} EntryState473;

extern GsIMAGE gFrenchModelVariant473_aImages[9];
extern EntryConfig473 gFrenchModelVariant473_aConfigs[];
void FrenchModelVariant385_DrawGrid(u8 *context);
void SpanishModelVariant473_DrawSpiral(u8 *context);
void FrenchModelVariant473_DrawRibs(u8 *context);
void SpanishModelVariant473_DrawSheets(u8 *context);
void SpanishModelVariant473_DrawFunnels(u8 *context);

#endif
