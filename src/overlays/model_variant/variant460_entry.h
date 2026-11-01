#ifndef MEMORIES_DECOMP_MODEL_VARIANT460_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT460_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../spanish_model_variant/variant477_panels.h"
#include "../../game/camera_view.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

/* One 0x38-byte command record of header 460's table at gModelVariant460_D1DCC + 0xFC. */
typedef struct {
    u8 color_a[4];
    u8 color_b[4];
    u8 color_c[4];
    u8 unknown_0C[4];
    u8 parts[2];
    u8 unknown_12[0xA];
    u32 panel_start;
    u32 quad_start;
    u8 unknown_24[8];
    u32 track_end;
    u8 unknown_30[8];
} Variant460Config;

/* A 0xA8-byte column of nine points with a shade per point. */
typedef struct {
    SVECTOR points[9];
    u8 color[9][4];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant460Column;

/* A 0x1A0-byte dome of two four-by-six point shells. */
typedef struct {
    SVECTOR outer[4][6];
    SVECTOR inner[4][6];
    u8 color[4];
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 size;
    s32 field_198, field_19C;
} Variant460Dome;

/* A 0xA0-byte pair of shaded sprites sharing one scale. */
typedef struct {
    u8 unknown_00[0x68];
    s32 size[2];
    u8 inner[2][4];
    u8 outer[2][4];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant460Pair;

/* A 0x90-byte eight-point double ring. */
typedef struct {
    SVECTOR a[8];
    SVECTOR b[8];
    u8 color_a[4];
    u8 color_b[4];
    s32 size;
    s32 field_8C;
} Variant460Ring;

/* A 0x90-byte five-spoke star with a square backing. */
typedef struct {
    SVECTOR a[6];
    SVECTOR b[5];
    SVECTOR square[4];
    u8 color_a[4];
    u8 color_b[4];
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} Variant460Star;

/* Header 460's 0x20BC-byte loader work area. */
typedef struct {
    Panel477 panels[2];
    QuadGroup477 groups[2];
    Variant460Column columns[16];
    Variant460Dome domes[3];
    Variant460Pair pair;
    ModelVariantSheet sheets[2];
    Variant460Ring rings[6];
    Variant460Ring halos[4];
    Variant460Star star;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 glow;
    POLY_FT4 flat[2];
    POLY_FT4 frame_poly;
    POLY_FT4 panel_poly;
    u8 unknown_1FEC[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR delta;
    DVECTOR projected;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step;
    s32 fade;
    Variant460Config *G32 config;
    u8 unknown_207C[4];
    GsCOORDUNIT *G32 parts[2];
    u8 unknown_2088[4];
    s16 field_208C, field_208E;
    s32 field_2090;
    s16 field_2094, field_2096;
    s32 field_2098;
    s32 enabled[2];
    s32 field_20A4;
    s32 phase;
    SVECTOR orbit;
    s32 tint;
    s16 slot, command;
} Variant460State;

extern u8 gModelVariant460_D1DCC[];
s32 ModelVariant460_HandleRequest(u8 *context, s32 command);
void SpanishModelVariant477_DrawPanels(u8 *context);
void SpanishModelVariant477_DrawQuadGroups(u8 *context);
#endif
