#ifndef MEMORIES_MODEL_VARIANT380_ENTRY_H
#define MEMORIES_MODEL_VARIANT380_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../french_model_variant/variant337_entry.h"

/* The 0x40-byte stack block below the projection locals; only its last two
 * halfwords, the texture CLUT words, are written. */
typedef struct {
    u8 unknown_00[60];
    u16 texture_cluts[2];
} Variant380EntryStack;

/* One 0x44-byte command row at D_8013D970 + 0x38. */
typedef struct {
    u8 unknown_00[8];
    s32 parts[5];
    u8 unknown_1C[8];
    s32 count;
    u8 unknown_28[4];
    u32 start, end;
    u8 unknown_34[0x10];
} Variant380EntryConfig;

/* Up to five 0x2D4-byte beams at the start of the work area. */
typedef struct {
    u8 unknown_000[0x1A0];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_1B8, field_1BA, field_1BC;
    u8 unknown_1BE[0x228 - 0x1BE];
    VECTOR velocity;
    u8 unknown_238[0x2D4 - 0x238];
} Variant380EntryBeam;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant380EntryBeam beams[5];
    ModelVariantSheetSet sheets[5];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[6];
    u8 unknown_19B8[0x10];
    MATRIX matrices[5];
    SVECTOR target;
    VECTOR targets[5];
    VECTOR directions[5];
    s16 screen_dx[5], screen_dy[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, span, fade;
    Variant380EntryConfig *G32 config;
    u32 field_1B58;
    GsCOORDUNIT *G32 parts[5];
    s16 field_1B70[5], field_1B7A[5], field_1B84[5], field_1B8E[5];
    s16 field_1B98, field_1B9A, width;
    s32 field_1BA0[5], field_1BB4[5], field_1BC8[5], field_1BDC[5];
    s32 field_1BF0, field_1BF4, phase;
    u8 unknown_1BFC[8];
    s32 ambient;
    s16 slot, command;
} Variant380EntryState;

extern u8 D_8013D970[];
void func_8013BDE8(SVECTOR *point);
void func_8013CAB8(SVECTOR *point);
void func_8013D08C(SVECTOR *point);
s32 func_8013B004(SVECTOR *point, s32 command);

#endif
