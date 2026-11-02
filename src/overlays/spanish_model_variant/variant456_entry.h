#ifndef MEMORIES_SPANISH_MODEL_VARIANT456_ENTRY_H
#define MEMORIES_SPANISH_MODEL_VARIANT456_ENTRY_H
#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x2C-byte row of header 456's command table. */
typedef struct {
    CVECTOR web, color, secondary;
    u8 part, unknown_0D[7];
    u32 sheets_start, lines_start;
    u8 unknown_1C[8];
    u32 phase_start, track_end;
} Variant456EntryConfig;

/* A centre and five points on an ellipse, a colour and the sweep words. */
typedef struct {
    SVECTOR point[6];
    CVECTOR color;
    s32 field_34, field_38, field_3C, field_40, field_44, field_48, field_4C;
} Variant456EntryFan;

/* A 0xF0-byte trail: two colours, a scale, three phases and a travelling
 * target with its offset from the part. */
typedef struct {
    u8 unknown_00[0x60];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_78, field_7A;
    s32 field_7C, field_80;
    u8 unknown_84[0x3C];
    s32 phase[3];
    s32 completed;
    VECTOR target;
    VECTOR delta;
} Variant456EntryTrail;

/* A 0xE0-byte ribbon (see Ribbon456): colours, scale, heading and phases. */
typedef struct {
    u8 unknown_00[0x60];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_78, field_7A;
    s32 field_7C;
    u8 unknown_80[0x18];
    s32 heading[3];
    u8 unknown_A4[0x28];
    s32 phase[3];
    s32 field_D8, field_DC;
} Variant456EntryRibbon;

/* A 0x108-byte line: colours, scale, heading and seventeen phases. */
typedef struct {
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_18, field_1A;
    s32 field_1C;
    u8 unknown_20[0x88];
    s32 heading[3];
    u8 unknown_B4[4];
    s32 phase[17];
    s32 field_FC, field_100, field_104;
} Variant456EntryLine;

/* A 0xC0-byte sheet of four quads with a travelling target. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
    VECTOR target;
    VECTOR delta;
    s32 field_B8, field_BC;
} Variant456EntrySheet;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} Variant456EntryStreamer;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant456EntryFan fans[2];
    Variant456EntryTrail trails[2];
    Variant456EntryRibbon ribbons[2];
    Variant456EntryLine lines[16];
    Variant456EntrySheet sheets[8];
    Variant456EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], glow;
    POLY_FT4 flat[2];
    POLY_FT4 flat_extra[2];
    u8 unknown_22A4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    s16 screen_x, screen_y;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant456EntryConfig *G32 config;
    u8 unknown_2334[4];
    GsCOORDUNIT *G32 part;
    s16 field_233C, field_233E, field_2340, field_2342;
    s32 field_2344, field_2348, field_234C, field_2350, field_2354;
    s32 field_2358, field_235C, field_2360, field_2364, field_2368;
    s32 phase;
    u8 unknown_2370[8];
    s32 tint;
    s16 slot, command;
} Variant456EntryState;

extern u8 gSpanishModelVariant456_D3D00[];
void SpanishModelVariant456_DrawSprays(u8 *context);
void SpanishModelVariant456_DrawSheets(u8 *context);
#endif
