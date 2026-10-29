#ifndef MEMORIES_MODEL_VARIANT464_ENTRY_H
#define MEMORIES_MODEL_VARIANT464_ENTRY_H
#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, field_1C, grow_start, grow_end;
    u8 unknown_28[0xC];
} Variant464EntryConfig;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag, target;
} Variant464EntryProjection;

/* Two 17-point circles, the outer one raised by 64. */
typedef struct {
    SVECTOR inner[17], outer[17];
    s32 size, field_114;
} Variant464EntryFunnel;

/* A 0x8EC-byte band of 33 steps with two colours per step. */
typedef struct {
    u8 unknown_000[0x420];
    s32 amount[33], offset[33];
    CVECTOR color[33], secondary[33];
    VECTOR scale;
    s16 field_640, field_642;
    s32 field_644;
    u8 unknown_648[0x108];
    s32 heading[3];
    u8 unknown_75C[0x190];
} Variant464EntryBand;

/* Nine 17-point rings of a dome, each with its own colour. */
typedef struct {
    SVECTOR points[9][17];
    CVECTOR color[9];
} Variant464EntryDome;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x24];
} Variant464EntrySpiral;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} Variant464EntryStreamer;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant464EntryFunnel funnels[2];
    Variant464EntryBand bands[1];
    Variant464EntryDome dome;
    Variant464EntrySpiral spirals[12];
    ModelVariantSheet sheets[2];
    Variant464EntryStreamer streamers[2];
    u8 unknown_1DD0[0x34];
    POLY_GT4 special;
    u8 unknown_1E38[0x34];
    POLY_GT4 beams[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    u8 unknown_1FCC[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    Variant464EntryConfig *G32 config;
    u32 field_2048;
    GsCOORDUNIT *G32 part;
    s16 field_2050, field_2052;
    s32 width, field_2058, field_205C, field_2060, field_2064;
    s32 field_2068, field_206C, field_2070, field_2074, field_2078;
    s32 field_207C, field_2080, progress;
    s16 field_2088, field_208A, field_208C, field_208E;
    s32 field_2090, field_2094, field_2098, field_209C;
    CVECTOR inner, outer;
    s32 field_20A8, field_20AC, field_20B0, field_20B4, field_20B8, phase;
    s32 field_20C0, field_20C4;
    SVECTOR camera;
    s32 ambient;
    s16 slot, command;
} Variant464EntryState;

extern u8 D_8013CE0C[];
void func_8013BE3C(u8 *context);
void func_8013C708(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
