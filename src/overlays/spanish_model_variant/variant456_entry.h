#ifndef MEMORIES_SPANISH_MODEL_VARIANT456_ENTRY_H
#define MEMORIES_SPANISH_MODEL_VARIANT456_ENTRY_H
#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x2C-byte row of header 456's command table. */
typedef struct {
    CVECTOR web, color, secondary;
    u8 part, unknown_0D[7];
    u32 sheets_start, lines_start;
    u32 fade_start, fade_end;
    u32 phase_start, track_end;
} Variant456EntryConfig;

/* A centre and five points on an ellipse, a colour and the sweep words. */
typedef struct {
    SVECTOR point[6];
    CVECTOR color;
    s32 field_34, field_38, field_3C, field_40, field_44, field_48, field_4C;
} Variant456EntryFan;

/* A 0xF0-byte trail: a three-point spine and a copy of it moved along the
 * view, both projected, the per-point screen angle and projected width, the
 * colours and scale the entry seeds, per point the depth and the width's
 * screen offset, the phases, a completion flag, the target and the step
 * toward it. The twin-ribbon helper scales each projected width by
 * width_scale / 1024. */
typedef struct {
    SVECTOR a[3];
    ModelVariantScreenPoint sa[3];
    s32 angle[3];
    SVECTOR b[3];
    ModelVariantScreenPoint sb[3];
    s32 width[3];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_78, field_7A;
    s32 field_7C, width_scale;
    u8 unknown_84[0x24];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    s32 phase[3];
    s32 completed;
    VECTOR target;
    VECTOR delta;
} Variant456EntryTrail;

/* A 0xE0-byte ribbon: a three-point spine and a copy of it moved along the
 * view, both projected, the per-point screen angle and projected width, the
 * colours, scale and heading the entry seeds, per point the depth and the
 * width's screen offset, and the phases. The ribbon drawer scales each
 * projected width by width_scale / 1024. */
typedef struct {
    SVECTOR a[3];
    long sa[3];
    s32 angle[3];
    SVECTOR b[3];
    long sb[3];
    s32 width[3];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_78, field_7A;
    s32 field_7C;
    u8 unknown_80[0x18];
    s32 heading[3];
    u8 unknown_A4[0x10];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    s32 phase[3];
    s32 field_D8, width_scale;
} Variant456EntryRibbon;

/* A 0x108-byte line (a spray of seventeen points): colours, scale, heading,
 * the per-point growth the entry staggers below zero and the spray drawer
 * grows to 1024, the spin and the hidden flag. */
typedef struct {
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_18, field_1A;
    s32 field_1C;
    u8 unknown_20[0x88];
    s32 heading[3];
    u8 unknown_B4[4];
    s32 growth[17];
    s32 spin, hidden, field_104;
} Variant456EntryLine;

/* A 0xC0-byte sheet of four quads with a travelling target. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
    VECTOR target;
    VECTOR delta;
    s32 faded, fade;
} Variant456EntrySheet;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant456EntryFan fans[2];
    Variant456EntryTrail trails[2];
    Variant456EntryRibbon ribbons[2];
    Variant456EntryLine lines[16];
    Variant456EntrySheet sheets[8];
    ModelVariantStreamer streamers[2];
    ModelVariantPackets packets;
    POLY_GT4 glow;
    POLY_FT4 flat[2];
    POLY_FT4 flat_extra[2];
    u8 unknown_22A4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    s16 screen_x, screen_y;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant456EntryConfig *config;
    u8 unknown_2334[4];
    GsCOORDUNIT *part;
    s16 field_233C, field_233E, field_2340, field_2342;
    s32 field_2344;
    ModelVariantStreamerParams streamer;
    /* The fan helper turns the fans by spin, advancing it while they fade. */
    s32 spray_wave, spray_phase, field_2360, field_2364, spin;
    ModelVariantEntryTail tail;
} Variant456EntryState;

extern GsIMAGE gSpanishModelVariant456_aImages[9];
extern Variant456EntryConfig gSpanishModelVariant456_aConfigs[];
void SpanishModelVariant456_DrawSprays(Variant456EntryState *context);
void SpanishModelVariant456_DrawSheets(Variant456EntryState *context);
#endif
