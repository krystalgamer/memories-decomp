#ifndef FRENCH_MODEL_VARIANT427_ENTRY_H
#define FRENCH_MODEL_VARIANT427_ENTRY_H

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
#include "../spanish_model_variant/variant427_mesh.h"
#include "../french_model_variant/variant427_streamers.h"

typedef struct {
    u8 unknown_00[0x10], parts[3], unknown_13[5];
    u32 draw_start;
    u8 unknown_1C[0x10];
} EntryConfig427;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag, target;
} EntryProjection427;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR colors[2];
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C, progress;
    u8 unknown_244[0x88];
    VECTOR position;
    u8 unknown_2DC[0xCC];
} EntryRibbon427;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x24];
} EntrySpiral427;

typedef struct {
    Mesh427 meshes[3];
    EntryRibbon427 ribbons[3];
    EntrySpiral427 spirals[12];
    ModelVariantSheet sheets[6];
    Streamer427 streamers[2];
    ModelVariantCurtain curtains[9];
    POLY_GT4 extra, curtain_packet;
    u8 unknown_34B8[0x34];
    POLY_GT4 special;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    u8 unknown_3618[0x10];
    MATRIX matrices[3];
    SVECTOR target;
    VECTOR origins[3], directions[3], ribbon_directions[3];
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig427 *G32 config;
    u32 field_3754;
    GsCOORDUNIT *G32 parts[3];
    s16 field_3764, field_3766;
    s32 width, field_376C, size, field_3774, field_3778, field_377C;
    s32 intensity, field_3784, field_3788, progress;
    s16 field_3790, field_3792, field_3794, field_3796;
    s32 field_3798, field_379C, field_37A0, field_37A4;
    CVECTOR inner, outer;
    s32 field_37B0, field_37B4, field_37B8, field_37BC, field_37C0, phase;
    s32 field_37C8, field_37CC;
    u8 unknown_37D0[8];
    s32 ambient;
    s16 slot, command;
} EntryState427;

extern u8 D_8013EACC[];
void func_8013BFB4(u8 *context);
void func_8013C57C(u8 *context);
void func_8013CFC4(u8 *context);
void func_8013D8A0(u8 *context);
void func_8013E5BC(u8 *context);

#endif
