#ifndef MEMORIES_FRENCH_MODEL_VARIANT412_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT412_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "variant412_trails.h"

typedef struct {
    u8 unknown_00[4];
    u8 root_part, parts[6], unknown_0B[5];
    u16 part_count, unknown_12;
    u32 capture_start, capture_stop, duration;
} Variant412EntryConfig;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant412Trail trails[1];
    Variant402EntryRecord records[1];
    Variant402EntryRing rings[2];
    u8 unknown_13E0[0x2C0];
    Family402Band bands[2];
    u8 unknown_18D0[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra, band_packet;
    POLY_FT4 flat_textured[2];
    u8 unknown_1A64[0x4C];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, current_animation_frame, elapsed, animation_frame;
    s32 step, captured_count, field_1B1C;
    u32 fade;
    Variant412EntryConfig *G32 config;
    u32 field_1B28;
    GsCOORDINATE2 *G32 root_part;
    GsCOORDINATE2 *G32 parts[6];
    s32 part_index;
    CVECTOR inner, outer;
    s16 field_1B54, field_1B56, field_1B58, field_1B5A;
    s16 width, field_1B5E, field_1B60, field_1B62;
    s32 phase, field_1B68;
    u8 unknown_1B6C[8];
    s32 tint;
    s16 slot, command;
} Variant412EntryState;

extern u8 gFrenchModelVariant412_D27A8[];
void FrenchModelVariant412_DrawTrails(u8 *context);

#endif
