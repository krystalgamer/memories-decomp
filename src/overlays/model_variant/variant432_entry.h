#ifndef MEMORIES_MODEL_VARIANT432_ENTRY_H
#define MEMORIES_MODEL_VARIANT432_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../spanish_model_variant/variant437_sheet.h"
#include "variant437_streamers.h"

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant432EntryProjection;

/* One 0x24-byte command row at D_8013DB64 + 0xFC. */
typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    s32 start, scale_start, scale_end;
    u8 unknown_18[0xC];
} Variant432EntryConfig;

/* Four 0x380-byte ribbons at the start of the work area. */
typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x130];
    s32 start, end, state, count;
} Variant432EntryRibbon;

typedef struct {
    u8 unknown_00[0x6C];
    s32 progress[3];
    CVECTOR inner[3], outer[3];
    VECTOR scale;
    s16 field_A0, field_A2;
    s32 field_A4;
    u8 unknown_A8[0xC];
} Variant432EntryRecord;

typedef struct {
    Variant432EntryRibbon ribbons[4];
    Sheet437 sheets[1];
    Variant432EntryRecord records[1];
    Variant437Streamer streamers[3];
    u8 unknown_1698[0x318];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2], extra[2];
    u8 unknown_1AF8[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    MATRIX origin_a, origin_b;
    VECTOR direction;
    s32 factor, field_1B94, scale, spread;
    VECTOR delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    s32 elapsed;
    u32 animation_frame;
    s32 step;
    s32 span, fade;
    Variant432EntryConfig *G32 config;
    u32 field_1BF8;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1C08, field_1C0A;
    s32 field_1C0C, field_1C10, field_1C14, field_1C18, spin;
    s16 field_1C20, field_1C22;
    s32 length, rotation_x, rotation_y, rotation_z;
    s32 phase_a, phase_b, phase;
    u8 unknown_1C40[8];
    s32 tint;
    s16 slot, command;
} Variant432EntryState;

extern u8 D_8013DB64[];
void func_8013BD24(u8 *context);
void func_8013C244(u8 *context);
void func_8013CB78(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);

#endif
