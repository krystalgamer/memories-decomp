#ifndef MEMORIES_MODEL_VARIANT446_ENTRY_H
#define MEMORIES_MODEL_VARIANT446_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_effect_state.h"

/* The 0x4C-byte effect configuration the entry picks by command. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[5], unknown_15[0x13];
    s32 use_part_points;
    u8 unknown_2C[4];
    u32 start;
    u8 unknown_34[4];
    u32 phase3_frame, phase5_frame, part_frame;
    u8 unknown_44[8];
} Variant446EntryConfig;

/* One 0x2B0-byte ribbon: two colours, a scale, a growing progress and an
 * offset on a fan of angles around the root part. */
typedef struct {
    u8 unknown_000[0x1A0];
    u8 ca[4], cb[4];
    VECTOR scale;
    s32 field_1B8, field_1BC;
    s32 progress;
    s32 field_1C4, field_1C8, field_1CC;
    u8 unknown_1D0[0x68];
    VECTOR offset;
    u8 unknown_248[0x68];
} Variant446EntryRibbon;

/* Twenty-four sprites at work + 0x2710, each released at a target point. */
typedef struct {
    SVECTOR v0[24], v1[24], v2[24], v3[24], origin[24];
    CVECTOR color;
    u8 unknown_3C4[0x10];
    s32 delay[24];
    s32 scale[24];
    s32 field_494[24];
    s32 field_4F4[24];
    s32 done[24];
} Variant446EntrySprites;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant446EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant446EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant446EntryQuad;

/* Header 446's 0x4AC4-byte work area. */
typedef struct {
    Variant446EntryRibbon ribbons[5];
    Variant446EntryRibbon trails[5];
    ModelVariantSheetSet sheets[10];
    ModelVariantSheetSet panels[10];
    Variant446EntrySprites sprites;
    Variant446EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant446EntryBand bands[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant446EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
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
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant446EntryConfig *G32 config;
    u32 field_4A68;
    GsCOORDUNIT *G32 parts[5];
    s32 field_4A80, field_4A84, field_4A88, trail_delay, field_4A90, field_4A94, field_4A98;
    s16 field_4A9C, field_4A9E;
    s32 sweep;
    s16 field_4AA4, field_4AA6;
    s32 field_4AA8, field_4AAC, phase;
    SVECTOR offset;
    s32 tint;
    s16 slot, command;
} Variant446EntryState;

extern u8 gModelVariant446_D3BF4[];
void FrenchModelVariant463_DrawRibbons(u8 *context);
void FrenchModelVariant463_DrawPairedSheets(u8 *context);
void FrenchModelVariant463_DrawTrailRibbons(u8 *context);
void FrenchModelVariant463_DrawSheets(u8 *context);
#endif
