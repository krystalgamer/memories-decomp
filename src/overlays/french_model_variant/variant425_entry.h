#ifndef MEMORIES_FRENCH_MODEL_VARIANT425_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT425_ENTRY_H

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
#include "../spanish_model_variant/variant425_mesh.h"
#include "variant423_funnel.h"
#include "../model_variant/variant425_streamers.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, ribbon_start, travel_start, travel_end, fade_start, fade_end;
} EntryConfig425;

typedef struct {
    DVECTOR projected;
    long interpolation, flag, target;
} EntryProjection425;

/* A seventeen-point ribbon: its spine and a copy moved along the view, both
 * projected, and per point the screen angle, the projected width, the depth
 * and the width's screen offset. */
typedef struct {
    SVECTOR a[17];
    long sa[17];
    s32 angle[17];
    SVECTOR b[17];
    long sb[17];
    s32 width[17];
    CVECTOR colors[2];
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x88];
    VECTOR position;
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} EntryRibbon425;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x1C];
} EntrySpiral425;

typedef struct {
    Mesh425Rows mesh;
    EntryRibbon425 ribbons[1];
    EntrySpiral425 spirals[12];
    ModelVariantSheet sheets[2];
    Variant425Streamer streamers[1];
    Funnel423 funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1590[0x34];
    POLY_GT4 special;
    u8 unknown_15F8[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    u8 unknown_1724[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig425 *config;
    u32 field_17A0;
    GsCOORDUNIT *part;
    s16 field_17A8, field_17AA;
    s32 width, field_17B0, size, field_17B8, field_17BC, field_17C0;
    s32 intensity, field_17C8, field_17CC, progress, travel;
    s16 spiral_angle, field_17DA, spiral_size, field_17DE;
    s32 streamer_length, streamer_rot_x, streamer_rot_y, streamer_rot_z;
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
