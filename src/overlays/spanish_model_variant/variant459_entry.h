#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT459_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT459_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x48-byte command record of the table at D_8013F548 + 0xFC. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u8 parts[6], unknown_16[2];
    u16 vertices[3], unknown_1E[3];
    u16 count, spiral;
    u8 unknown_28[4];
    s32 curtains, mode;
    u32 start;
    u8 unknown_38[0x10];
} Variant459EntryConfig;

/* Three seventeen-point loops, two colours, a scale and a growth word. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    u8 color_a[4], color_b[4];
    s32 scale, growth;
} Variant459EntryCurtain;

/* Two seventeen-point loops with a scale and a growth word. */
typedef struct {
    SVECTOR a[17], b[17];
    s32 scale, growth;
} Variant459EntryRing;

/* Four rows of six near and far points, a colour and a scale. */
typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale, field_198, field_19C;
} Variant459EntryWeb;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} Variant459EntryGlow;

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
} Variant459EntryBand;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant459EntryCurtain curtains[5];
    Variant459EntryRing rings[4];
    Variant459EntryWeb webs[3];
    Variant459EntryGlow glows[12];
    u8 rays[0x360];
    Variant459EntryBand bands[6];
    ModelVariantSheet sheets[13];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 plain;
    POLY_GT4 ray;
    POLY_FT4 flat_textured[2];
    u8 unknown_2E80[0x4C];
    MATRIX matrices[6];
    VECTOR points[6];
    VECTOR directions[6];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_3094, field_3096, field_3098, field_309A;
    Variant459EntryConfig *G32 config;
    u8 unknown_30A0[4];
    GsCOORDUNIT *G32 parts[6];
    s16 field_30BC, field_30BE;
    CVECTOR color_30C0, color_30C4;
    s32 field_30C8;
    s16 field_30CC, field_30CE;
    s32 field_30D0, field_30D4, field_30D8;
    s32 phase;
    s32 field_30E0;
    u8 unknown_30E4[8];
    s32 tint;
    s16 slot, command;
} Variant459EntryState;

extern u8 D_8013F548[];
void func_8013C9AC(u8 *context);
void func_8013D604(u8 *context);
void func_8013E224(u8 *context);
void func_8013EA24(u8 *context);
void func_8013EFE0(u8 *context);
typedef char Variant459EntryConfigSize[(sizeof(Variant459EntryConfig) == 0x48) ? 1 : -1];
typedef char Variant459EntryStateSize[(sizeof(Variant459EntryState) == 0x30F4) ? 1 : -1];
#endif
