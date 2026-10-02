#ifndef MEMORIES_FRENCH_MODEL_VARIANT337_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT337_ENTRY_H
#include "../../types.h"
#include "../../overlays/model_variant/model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

/* Entry initialization views; the accepted renderers retain their own views. */
typedef struct {
    CVECTOR inner, outer;
    u8 part, unknown_09[7];
    u32 start, end, field_18, fade_start, fade_end;
} Variant337EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x2C8 - 0x240];
    VECTOR velocity;
    u8 unknown_2D8[0x3A4 - 0x2D8];
} Variant337EntryRecord;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale, field_8C, field_90;
    u8 unknown_94[4];
} Variant337EntryRing;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x378 - 0x2AC];
} Variant337EntryStreamer;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant337EntryRecord records[1];
    Variant337EntryRing rings[2];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    u8 unknown_CBC[0xCCC - 0xCBC];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant337EntryConfig *G32 config;
    u32 field_D38;
    GsCOORDUNIT *G32 part;
    s16 field_D40, field_D42, field_D44, width;
    s32 field_D48, field_D4C, field_D50, field_D54, field_D58;
    s32 field_D5C, field_D60, field_D64, field_D68, phase;
    u8 unknown_D70[8];
    s32 field_D78;
    s16 slot, command;
} Variant337EntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant337EntryProjection;

extern u8 D_8013CF5C[];
void func_8013B9A4(u8 *context);
void func_8013C278(u8 *context);
void func_8013C738(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
