#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT438_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x48-byte command record of the table at D_8013F868 + 0xFC. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
    s32 offset[3];
    u16 sheet_size, unknown_3E;
    u8 unknown_40[4];
    s32 size;
} Variant438EntryConfig;

/* Three seventeen-point loops, two colours, a scale and a growth word. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    u8 color_a[4], color_b[4];
    s32 scale, growth;
} Variant438EntryCurtain;

/* Two seventeen-point loops with a scale and a growth word. */
typedef struct {
    SVECTOR a[17], b[17];
    s32 scale, growth;
} Variant438EntryRing;

/* Four rows of six near and far points, a colour and a scale. */
typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale, field_198, field_19C;
} Variant438EntryWeb;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} Variant438EntryGlow;

/* The 0x1C8-byte ModelVariantBand with the words its pad holds. */
typedef struct {
    u8 unknown_000[0x144];
    CVECTOR inner[9];
    CVECTOR outer[9];
    s32 scale[3];
    u8 unknown_198[4];
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} Variant438EntryBand;

/* Two eight-point loops, two colours, an angle and a word. */
typedef struct {
    SVECTOR a[8], b[8];
    CVECTOR color_a, color_b;
    s32 angle, field_8C;
} Variant438EntrySpoke;

/* A six-point fan, five rays, four corners and two colours. */
typedef struct {
    SVECTOR points[6];
    SVECTOR rays[5];
    SVECTOR corners[4];
    CVECTOR color_a, color_b;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} Variant438EntryStar;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant438EntryCurtain curtains[5];
    Variant438EntryRing rings[1];
    Variant438EntryWeb webs[3];
    Variant438EntryGlow glows[12];
    u8 rays[0x360];
    Variant438EntryBand bands[1];
    ModelVariantSheet sheets[2];
    Variant438EntrySpoke spokes[6];
    Variant438EntrySpoke petals[4];
    Variant438EntryStar stars[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 ray;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_21F8[0x4C];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_22AC, field_22AE, field_22B0, field_22B2;
    Variant438EntryConfig *G32 config;
    u8 unknown_22B8[4];
    GsCOORDUNIT *G32 parts[3];
    s16 field_22C8, field_22CA;
    CVECTOR color_22CC, color_22D0;
    s32 field_22D4;
    s16 field_22D8, field_22DA;
    s32 field_22DC, field_22E0, field_22E4;
    s32 phase;
    s32 field_22EC;
    u8 unknown_22F0[8];
    s32 tint;
    s16 slot, command;
} Variant438EntryState;

extern u8 D_8013F868[];
void func_8013C614(u8 *context);
void func_8013CB5C(u8 *context);
void func_8013DA34(u8 *context);
void func_8013E638(u8 *context);
void func_8013EDE8(u8 *context);
void func_8013F300(u8 *context);
typedef char Variant438EntryConfigSize[(sizeof(Variant438EntryConfig) == 0x48) ? 1 : -1];
typedef char Variant438EntryStateSize[(sizeof(Variant438EntryState) == 0x2300) ? 1 : -1];
#endif
