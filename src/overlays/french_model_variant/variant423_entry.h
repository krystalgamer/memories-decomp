#ifndef MEMORIES_FRENCH_MODEL_VARIANT423_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT423_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../spanish_model_variant/variant423_mesh.h"
#include "variant423_funnel.h"
#include "../model_variant/variant458_streamers.h"
#include "../../game/model_get_frame_step.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, grow_end, field_20, fade_start, fade_end;
} EntryConfig423;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR colors[2];
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x88];
    VECTOR position;
    u8 unknown_2D8[0x88];
} EntryRibbon423;

typedef struct {
    Mesh423Rows mesh;
    EntryRibbon423 ribbons[1];
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[2];
    Variant458Streamer streamers[2];
    Funnel423 funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1B94[0x34];
    POLY_GT4 special;
    u8 unknown_1BFC[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    u8 unknown_1D28[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig423 *config;
    u32 field_1DA4;
    GsCOORDUNIT *part;
    s16 field_1DAC, field_1DAE;
    s32 width, field_1DB4, size, field_1DBC, field_1DC0, field_1DC4;
    s32 intensity, field_1DCC, field_1DD0, progress;
    s16 spiral_angle, field_1DDA, spiral_size, field_1DDE;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, field_1E00, field_1E04, field_1E08, phase;
    s32 field_1E10, field_1E14;
    u8 unknown_1E18[8];
    s32 ambient;
    s16 slot, command;
} EntryState423;

extern GsIMAGE gFrenchModelVariant423_aImages[9];
extern EntryConfig423 gFrenchModelVariant423_aConfigs[];
void SpanishModelVariant423_DrawMesh(u8 *context);
void SpanishModelVariant423_DrawSpiral(u8 *context);
void FrenchModelVariant423_DrawRibbon(u8 *context);
void SpanishModelVariant423_DrawSheets(u8 *context);
void FrenchModelVariant423_DrawFunnel(u8 *context);

#endif
