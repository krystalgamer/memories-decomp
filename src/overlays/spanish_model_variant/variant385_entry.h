#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT385_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT385_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x4C-byte command record of the table at D_8013DEB4 + 0xFC. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u8 unknown_10[4];
    s32 inner_radius, outer_radius;
    u8 unknown_1C[0xC];
    s32 size;
    u32 start;
    u8 unknown_30[0x10];
    s32 offset[3];
} Variant385EntryConfig;

typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale, field_198, field_19C;
} Variant385EntryWeb;

typedef struct {
    u8 unknown_00[0x6C];
    s16 field_6C, field_6E;
    s32 field_70;
    u8 unknown_74[0x18];
} Variant385EntryTube;

typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} Variant385EntrySheet;

typedef struct {
    SVECTOR inner[17], outer[17];
    s32 angle, field_114;
} Variant385EntryCurtain;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant385EntryWeb webs[3];
    Variant385EntryTube tubes[1];
    Variant385EntrySheet sheets[2];
    u8 unknown_68C[0x300];
    Variant385EntryCurtain curtains[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 plain;
    POLY_GT4 paged;
    POLY_FT4 flat_textured[2];
    u8 unknown_F4C[0x4C];
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
    Variant385EntryConfig *G32 config;
    u8 unknown_1004[4];
    GsCOORDUNIT *G32 parts[3];
    s32 index;
    u8 color[3], unknown_101B;
    u8 field_101C, field_101D, field_101E, unknown_101F;
    s16 field_1020, field_1022, field_1024, field_1026;
    s16 field_1028, field_102A, field_102C, field_102E;
    s32 phase;
    s32 field_1034;
    u8 unknown_1038[8];
    s32 tint;
    s16 slot, command;
} Variant385EntryState;

extern u8 D_8013DEB4[];
void func_8013BF5C(u8 *context);
void func_8013C478(u8 *context);
void func_8013CB00(u8 *context);
void func_8013D1B8(u8 *context);
void func_8013D6B4(u8 *context);
typedef char Variant385EntryConfigSize[(sizeof(Variant385EntryConfig) == 0x4C) ? 1 : -1];
typedef char Variant385EntryStateSize[(sizeof(Variant385EntryState) == 0x1048) ? 1 : -1];
#endif
