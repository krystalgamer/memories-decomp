#ifndef FRENCH_MODEL_VARIANT469_ENTRY_H
#define FRENCH_MODEL_VARIANT469_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../french_model_variant/variant582_arcs.h"
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
    CVECTOR outer, inner, arc_color;
    u8 unknown_0C[4], parts[3], unknown_13[0x11];
    u32 draw_start, arc_limit;
    u8 unknown_2C[8];
    u32 matrix_end;
    u8 unknown_38[8];
} EntryConfig469;

typedef struct {
    SVECTOR points[3][13];
    CVECTOR colors[13];
    s32 progress, state;
} EntrySheet469;

typedef struct {
    u8 unknown_000[0x160];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_178, field_17C, count, field_184, state, length;
    u8 unknown_190[0x58];
    VECTOR position;
    u8 unknown_1F8[0x58];
} EntryRibbon469;

typedef struct {
    SVECTOR v0[24], v1[24], v2[24], v3[24], positions[24];
    CVECTOR color;
    u8 unknown_3C4[0x10];
    s32 size[24], progress[24], field_494[24], field_4F4[24], initialized[24];
} EntryRing469;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR colors[9];
    s32 field_6C, field_70, field_74, field_78, progress[9], field_A0, field_A4;
} EntryTrail469;

typedef struct {
    u8 unknown_00[0x68];
    s32 progress[2];
    CVECTOR outer[2], inner[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} EntryBand469;

typedef struct {
    ModelVariantSheet sheet;
    s32 field_98;
} EntryQuad469;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, state;
} EntrySmallRing469;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection469;

typedef struct {
    Arcs582Group arcs[4];
    EntrySheet469 sheets[5];
    EntryRibbon469 ribbons[5];
    EntryRing469 ring;
    EntryTrail469 trails[16];
    EntryBand469 bands[1];
    EntryQuad469 quads[10];
    EntrySmallRing469 rings_a[6], rings_b[4];
    u8 unknown_32A0[0x90];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_F4 mask;
    POLY_GT4 textured[2];
    u8 unknown_33F0[0x34];
    POLY_FT4 flat[2], ring_packet;
    GsGLINE line;
    u8 unknown_34B0[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    VECTOR target_center, positions[5], path_origins[5], path_targets[5], paths[5];
    VECTOR direction, line_direction;
    DVECTOR screen_delta;
    VECTOR ring_positions[24], ring_directions[24], view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig469 *G32 config;
    u32 field_399C;
    GsCOORDUNIT *G32 parts[3];
    s32 angle, scale, wave, intensity, field_39BC, field_39C0;
    s16 field_39C4, field_39C6;
    s32 field_39C8;
    s16 field_39CC, field_39CE;
    s32 field_39D0, field_39D4, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState469;

extern u8 D_8013E4BC[];
void func_8013C560(u8 *context);
void func_8013CB98(u8 *context);
void func_8013E068(u8 *context);

#endif
