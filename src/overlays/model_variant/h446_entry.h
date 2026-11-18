#ifndef MEMORIES_MODEL_VARIANT446_ENTRY_H
#define MEMORIES_MODEL_VARIANT446_ENTRY_H
#include "../../types.h"
#include "../model_variant/e503_h320_entry_pal.h"
#include "../../game/model_camera_effects.h"

/* The 0x4C-byte effect configuration the entry picks by command. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[5], unknown_15[0xB];
    s32 first_count, paired;
    s32 use_part_points;
    s32 mode;
    u32 start;
    u32 grow_end;
    u32 phase3_frame, phase5_frame, part_frame;
    u8 unknown_44[8];
} Variant446EntryConfig;

/* One 0x2B0-byte ribbon: a thirteen-point spine and its offset copy, both
 * projected, with per-point angle and width; two colours, a scale, a growing
 * progress and an offset on a fan of angles around the root part; and per
 * point the depth and screen offset. */
typedef struct {
    SVECTOR a[13];
    ModelVariantScreenPoint sa[13];
    s32 angle[13];
    SVECTOR b[13];
    ModelVariantScreenPoint sb[13];
    s32 width[13];
    CVECTOR ca;
    CVECTOR cb;
    VECTOR scale;
    s32 field_1B8, field_1BC;
    s32 progress;
    s32 field_1C4, state, width_scale;
    u8 unknown_1D0[0x68];
    VECTOR offset;
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} Variant446EntryRibbon;

/* Header 446's 0x4AC4-byte work area. */
typedef struct {
    Variant446EntryRibbon ribbons[5];
    Variant446EntryRibbon trails[5];
    ModelVariantSheetSet sheets[10];
    ModelVariantSheetSet panels[10];
    /* Twenty-four quads at work + 0x2710, each released at a target point. */
    ModelVariantQuadRing sprites;
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_4448[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR position;
    MATRIX part_matrices[5];
    VECTOR bases[5];
    VECTOR tips[5];
    VECTOR spans[5];
    VECTOR ground[5];
    VECTOR targets[5];
    VECTOR target_spans[5];
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[24];
    VECTOR offsets[24];
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant446EntryConfig *config;
    u32 field_4A68;
    GsCOORDUNIT *parts[5];
    s32 ring_angle, ring_scale, ring_wave, trail_delay, field_4A90, ribbon_wave, ribbon_phase;
    s16 field_4A9C, field_4A9E;
    s32 sweep;
    s16 field_4AA4, field_4AA6;
    s32 field_4AA8, field_4AAC, phase;
    SVECTOR offset;
    s32 tint;
    s16 slot, command;
} Variant446EntryState;

extern GsIMAGE gModelVariant446_aImages[9];
extern Variant446EntryConfig gModelVariant446_aConfigs[];
void FrenchModelVariant463_DrawRibbons(Variant446EntryState *context);
void FrenchModelVariant463_DrawPairedSheets(Variant446EntryState *context);
void FrenchModelVariant463_DrawTrailRibbons(Variant446EntryState *context);
void FrenchModelVariant463_DrawSheets(Variant446EntryState *context);
#endif
