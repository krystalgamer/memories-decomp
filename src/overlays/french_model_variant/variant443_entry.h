#ifndef FRENCH_MODEL_VARIANT443_ENTRY_H
#define FRENCH_MODEL_VARIANT443_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant443_streamers.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"

typedef struct {
    u8 inner[3][3], unknown_09[3];
    u8 outer[3][3], unknown_15[3];
    CVECTOR sphere_color;
    u8 unknown_1C[4], parts[4];
    u16 vertices[4];
    u8 unknown_2C[12];
    u32 start, timing[7];
} EntryConfig443;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x88];
    VECTOR position;
    u8 unknown_2D8[0x98];
} EntryRibbon443;

typedef struct {
    SVECTOR points[3][4];
    CVECTOR inner, outer;
    s32 initial, hidden;
} EntryFramebufferRing443;

typedef struct {
    u8 unknown_000[0x510];
    CVECTOR colors[9];
    VECTOR scale;
    s16 field_544, field_546;
    s32 field_548;
} EntryMesh443;

typedef struct {
    SVECTOR near[4][6], far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C, field_190;
    s32 scale, done, field_19C;
} EntrySphere443;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x1C];
} EntrySpiral443;

typedef struct {
    u8 unknown_000[0x144];
    CVECTOR inner[9], outer[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} EntryLongBand443;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_68, field_6A;
    s32 field_6C;
    u8 unknown_70[8];
} EntryShortBand443;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 initial, hidden;
} EntryRing443;

typedef struct {
    SVECTOR inner[6], outer[5], corners[4];
    CVECTOR inner_color, outer_color;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} EntryShape443;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection443;

typedef struct {
    EntryRibbon443 ribbons[3];
    EntryFramebufferRing443 rings[5];
    EntryMesh443 meshes[1];
    Streamer443 streamers[3];
    EntrySphere443 spheres[3];
    EntrySpiral443 spirals[12];
    u8 unknown_23D8[0x360];
    EntryLongBand443 long_bands[1];
    EntryShortBand443 bands[3];
    ModelVariantSheet sheets[5];
    EntryRing443 rings_a[6], rings_b[4];
    EntryShape443 shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2], additional[2];
    u8 unknown_350C[0x38];
    s32 stored_target[3];
    VECTOR vertices[2], positions[3], origin;
    SVECTOR target;
    VECTOR direction, paths[3];
    DVECTOR projected;
    s16 screen_x[3], screen_y[3];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame, elapsed, animation;
    s32 step, fade;
    EntryConfig443 *G32 config;
    u8 unknown_3638[0x14];
    s16 field_364C, field_364E;
    s32 factor, progress, field_3658, spin;
    s16 field_3660, field_3662, field_3664, field_3666;
    s32 field_3668, field_366C, field_3670, field_3674;
    s16 size, field_367A;
    s32 travel, length, rotation_x, rotation_y, rotation_z;
    s32 field_3690, field_3694, phase, field_369C;
    u8 unknown_36A0[8];
    s32 ambient;
    s16 slot, command;
} EntryState443;

extern u8 D_8013F5A8[];
void FrenchModelVariant443_DrawRibbons(u8 *context);
void func_8013D3AC(u8 *context);
void func_8013D98C(u8 *context);
void func_8013DF2C(u8 *context);
void func_8013E668(u8 *context);
void func_8013EEF0(u8 *context);

#endif
