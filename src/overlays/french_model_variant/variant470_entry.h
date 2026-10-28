#ifndef FRENCH_MODEL_VARIANT470_ENTRY_H
#define FRENCH_MODEL_VARIANT470_ENTRY_H

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
} EntryConfig470;

typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    CVECTOR color;
    u8 unknown_194[0x10];
    s32 size[10], done[10], field_1F4[10], initialized[10];
    MATRIX matrices[10];
    VECTOR captured[10], directions[10];
} EntryPair470;

typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    CVECTOR color;
    s32 size[10], angle[10], done[10], field_20C[10], initialized[10];
    u8 unknown_25C[0xA0];
} EntryGroup470;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR colors[9];
    s32 field_6C, field_70, field_74, field_78, progress[9], field_A0, field_A4;
} EntryTrail470;

typedef struct {
    u8 unknown_00[0x68];
    s32 progress[2];
    CVECTOR outer[2], inner[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} EntryBand470;

typedef struct {
    SVECTOR points[4][4];
    CVECTOR inner, outer;
    VECTOR scale;
} EntrySheet470;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, state;
} EntrySmallRing470;

typedef struct {
    SVECTOR origin, inner[5], outer[5], corners[4];
    CVECTOR inner_color, outer_color;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} EntryShape470;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection470;

typedef struct {
    EntryPair470 pairs[2];
    EntryGroup470 groups[1];
    EntryTrail470 trails[16];
    ModelVariantWebNarrow webs[3];
    EntryBand470 bands[1];
    EntrySheet470 sheets[2];
    EntrySmallRing470 rings_a[6], rings_b[4];
    EntryShape470 shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2], single_packet, pair_packet;
    GsGLINE line;
    u8 unknown_2574[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig470 *G32 config;
    u32 field_25F0;
    GsCOORDUNIT *G32 parts[2];
    u32 field_25FC;
    s16 field_2600, field_2602;
    u32 motion;
    s16 field_2608, field_260A;
    s32 field_260C;
    SVECTOR camera_offset;
    s32 ambient, field_261C, phase;
    s16 slot, command;
} EntryState470;

extern u8 D_8013CED0[];
void func_8013C510(u8 *context);
void func_8013C9F4(u8 *context);

#endif
