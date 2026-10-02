#ifndef MEMORIES_FRENCH_MODEL_VARIANT402_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT402_ENTRY_H
#include "../../types.h"
#include "variant402_bands.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4];
    u8 parts[3];
    u8 unknown_07[5];
    u16 part_count;
    u8 unknown_0E[2];
    u32 start;
} Variant402EntryConfig;

typedef struct {
    u8 unknown_00[0x48];
    s16 field_48, field_4A;
    s32 field_4C;
    u8 unknown_50[8];
} Variant402EntryRecord;

typedef struct {
    SVECTOR points[16];
    s32 field_80, field_84, field_88, field_8C;
} Variant402EntryRing;

typedef struct {
    Variant402EntryRecord records[1];
    Variant402EntryRing rings[2];
    u8 unknown_178[0x438 - 0x178];
    Family402Band bands[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra, band_packet;
    POLY_FT4 flat_textured[2];
    u8 unknown_7C8[0x814 - 0x7C8];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant402EntryConfig *G32 config;
    u32 field_880;
    GsCOORDUNIT *G32 parts[3];
    s32 part_index;
    CVECTOR inner, outer;
    s16 field_89C, field_89E, field_8A0, field_8A2;
    s16 width, field_8A6, field_8A8, field_8AA;
    s32 phase, field_8B0, field_8B4, field_8B8, field_8BC;
    s16 slot, command;
} Variant402EntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant402EntryProjection;

extern u8 D_8013D060[];
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
