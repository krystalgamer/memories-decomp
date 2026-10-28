#ifndef FRENCH_MODEL_VARIANT477_ENTRY_H
#define FRENCH_MODEL_VARIANT477_ENTRY_H

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
    u8 unknown_0C[4], parts[2], unknown_12[0xA];
    u32 draw_start, limit, field_24, field_28, matrix_end, move_start, move_end;
} EntryConfig477;

typedef struct {
    SVECTOR a[1], b[1], c[1], d[1], positions[1];
    CVECTOR color;
    u8 unknown_2C[0x10];
    s32 size[1], done[1], field_44[1], initialized[1];
    MATRIX matrices[1];
    VECTOR captured[1], directions[1];
} EntryPanel477;

typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    CVECTOR color;
    s32 size[10], angle[10], done[10], field_20C[10], initialized[10];
    u8 unknown_25C[0xA0];
} EntryGroup477;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR colors[9];
    s32 field_6C, field_70, field_74, field_78, progress[9], field_A0, field_A4;
} EntryTrail477;

typedef struct {
    u8 unknown_00[0x68];
    s32 progress[2];
    CVECTOR outer[2], inner[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} EntryBand477;

typedef struct {
    SVECTOR points[4][4];
    CVECTOR inner, outer;
    VECTOR scale;
} EntrySheet477;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, state;
} EntrySmallRing477;

typedef struct {
    SVECTOR origin, inner[5], outer[5], corners[4];
    CVECTOR inner_color, outer_color;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} EntryShape477;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection477;

typedef struct {
    EntryPanel477 panels[2];
    EntryGroup477 groups[2];
    EntryTrail477 trails[16];
    ModelVariantWebNarrow webs[3];
    EntryBand477 bands[1];
    EntrySheet477 sheets[2];
    EntrySmallRing477 rings_a[6], rings_b[4];
    EntryShape477 shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2], quads_packet, panels_packet;
    GsGLINE line;
    u8 unknown_2000[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig477 *G32 config;
    u32 field_207C;
    GsCOORDUNIT *G32 parts[2];
    u32 field_2088;
    s16 field_208C, field_208E;
    u32 field_2090;
    s16 field_2094, field_2096;
    s32 field_2098, enabled[2], field_20A4, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState477;

extern u8 D_8013CDC0[];
void func_8013C484(u8 *context);
void func_8013C958(u8 *context);

#endif
