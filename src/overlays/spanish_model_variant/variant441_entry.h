#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x30-byte command record of the table at D_8013F754 + 0xFC. */
typedef struct {
    u8 unknown_00[8];
    CVECTOR web;
    u8 unknown_0C[4];
    u8 parts[3], unknown_13[5];
    u16 count, unknown_1A;
    s32 start;
    u8 unknown_20[0xC];
    s32 end;
} Variant441EntryConfig;

/* Three seventeen-point loops with one colour per point. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR colors[17];
    s32 scale, growth;
} Variant441EntryHoop;

/* The 0x54C-byte tube with nine fixed colours and its scale words. */
typedef struct {
    u8 unknown_000[0x510];
    CVECTOR colors[9];
    s32 scale[3];
    u8 unknown_540[4];
    s16 field_544, field_546;
    s32 field_548;
} Variant441EntryTube;

typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale, field_198, field_19C;
} Variant441EntryWeb;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color;
    s32 scale[3];
    u8 unknown_58[4];
    s16 field_5C, field_5E;
    s32 field_60;
    u8 unknown_64[8];
} Variant441EntryRibbon;

typedef struct {
    SVECTOR a[17], b[17], c[17];
    u8 color_a[4], color_b[4];
    s32 scale, growth;
} Variant441EntryCurtain;

typedef struct {
    SVECTOR a[8], b[8];
    CVECTOR color_a, color_b;
    s32 angle, field_8C;
} Variant441EntrySpoke;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} Variant441EntryGlow;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant441EntryHoop hoops[3];
    Variant441EntryTube tubes[1];
    Variant441EntryWeb webs[3];
    Variant441EntryRibbon ribbons[1];
    ModelVariantSheet sheets[2];
    u8 rays[0x360];
    Variant441EntryCurtain curtains[5];
    Variant441EntrySpoke spokes[4];
    Variant441EntryGlow glows[12];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_2658[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    s32 frame;
    u32 animation_frame;
    s32 step, fade;
    Variant441EntryConfig *G32 config;
    u8 unknown_26E8[4];
    GsCOORDUNIT *G32 parts[3];
    s32 index;
    s16 field_26FC, field_26FE, field_2700, field_2702;
    s32 width, field_2708, field_270C;
    s16 field_2710, field_2712, progress, ribbon_width;
    s32 field_2718;
    s32 phase;
    s32 field_2720;
    u8 unknown_2724[8];
    s32 tint;
    s16 slot, command;
} Variant441EntryState;

extern u8 D_8013F754[];
void func_8013C73C(u8 *context);
void func_8013DA48(u8 *context);
void func_8013E060(u8 *context);
void func_8013F14C(u8 *context);
typedef char Variant441EntryConfigSize[(sizeof(Variant441EntryConfig) == 0x30) ? 1 : -1];
typedef char Variant441EntryStateSize[(sizeof(Variant441EntryState) == 0x2734) ? 1 : -1];
#endif
