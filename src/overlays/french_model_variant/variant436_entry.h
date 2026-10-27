#ifndef FRENCH_MODEL_VARIANT436_ENTRY_H
#define FRENCH_MODEL_VARIANT436_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../spanish_model_variant/variant436_sheets.h"
#include "variant436_streamers.h"

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection436;

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    s32 start, scale_start, scale_end;
    u8 unknown_18[0x18];
} EntryConfig436;

typedef struct {
    u8 unknown_00[0x6C];
    s32 progress[3];
    CVECTOR inner[3], outer[3];
    VECTOR scale;
    s16 field_A0, field_A2;
    s32 field_A4;
    u8 unknown_A8[0xC];
} EntryRecord436;

typedef struct {
    u8 unknown_000[0x120];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0xB0];
    s32 start, end, state, count;
} EntryRibbon436;

typedef struct {
    EntryRibbon436 ribbons[4];
    Sheet436 sheets[1];
    EntryRecord436 records[1];
    Streamer436 streamers[3];
    u8 unknown_1098[0x318];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2], extra[2];
    u8 unknown_14F8[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    MATRIX origin_a, origin_b;
    VECTOR direction;
    s32 factor, lift, scale, spread;
    VECTOR delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    s32 elapsed;
    u32 animation_frame;
    s32 step, span, fade;
    EntryConfig436 *G32 config;
    u32 field_15F8;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1608, field_160A;
    s32 field_160C, field_1610, field_1614, field_1618, spin;
    s16 field_1620, field_1622;
    s32 length, rotation_x, rotation_y, rotation_z;
    s32 phase_a, phase_b, phase;
    u8 unknown_1640[8];
    s32 tint;
    s16 slot, command;
} EntryState436;

extern u8 D_8013DFBC[];
void func_8013CAEC(u8 *context);
void func_8013C53C(u8 *context);
void func_8013D6C8(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);

#endif
