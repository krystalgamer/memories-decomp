#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x24-byte command record of the table at D_8013DE7C + 0xFC. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u32 sheet_start;
    u8 unknown_14[8];
    u32 track_end;
    u8 unknown_20[4];
} Variant388EntryConfig;

typedef struct {
    u8 unknown_00[0xB4];
    CVECTOR color;
    s32 scale[3];
    u8 unknown_C4[4];
    s16 field_C8, field_CA;
    s32 field_CC;
    u8 unknown_D0[0x28];
} Variant388EntryTube;

typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    CVECTOR inner, outer;
    s32 field_88, field_8C, field_90;
    u8 unknown_94[4];
} Variant388EntrySheet;

typedef struct {
    SVECTOR points[3][17];
    CVECTOR inner, outer;
    s32 progress, field_1A4;
} Variant388EntryFeedback;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR color, shade;
    s32 angle, field_8C;
} Variant388EntrySpoke;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR cb[2];
    CVECTOR ca[2];
    u8 unknown_60[0x24];
} Variant388EntryArm;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant388EntryTube tubes[1];
    Variant388EntrySheet sheets[2];
    u8 unknown_228[0x3A0];
    Variant388EntryFeedback feedback[5];
    Variant388EntrySpoke spokes[4];
    Variant388EntryArm arms[12];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 paged;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_17E0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    u32 frame;
    u32 animation_frame;
    s32 step, fade;
    Variant388EntryConfig *G32 config;
    u8 unknown_1870[4];
    GsCOORDUNIT *G32 parts[3];
    s32 index;
    s16 field_1884, field_1886, field_1888, field_188A;
    s16 field_188C, field_188E, field_1890, field_1892;
    s32 field_1894;
    s32 phase;
    s32 field_189C;
    u8 unknown_18A0[8];
    s32 tint;
    s16 slot, command;
} Variant388EntryState;

extern u8 D_8013DE7C[];
void func_8013BCD4(u8 *context);
void func_8013C344(u8 *context);
void func_8013C9A4(u8 *context);
void func_8013CE9C(u8 *context);
void func_8013D410(u8 *context);
typedef char Variant388EntryConfigSize[(sizeof(Variant388EntryConfig) == 0x24) ? 1 : -1];
typedef char Variant388EntryStateSize[(sizeof(Variant388EntryState) == 0x18B0) ? 1 : -1];
#endif
