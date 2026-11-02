#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT405_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT405_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_get_frame_step.h"

/* One 0x20-byte command record of the table at gSpanishModelVariant405_D1FBC + 0x38. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[1], unknown_05[7];
    s32 field_0C;
    u32 field_10, field_14, field_18, field_1C;
} Variant405EntryConfig;

typedef struct {
    SVECTOR a[2], b[2], c[2];
    u8 unknown_30[0x18];
    u8 field_48, field_49, field_4A, unknown_4B;
    s32 field_4C, field_50, field_54;
    u8 unknown_58[4];
    s16 field_5C, field_5E, field_60, field_62;
    s32 field_64, field_68;
    u8 unknown_6C[8];
} Variant405EntryRing;

typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 field_88, field_8C, field_90;
    u8 unknown_94[4];
    s32 field_98;
} Variant405EntrySheet;

typedef struct {
    SVECTOR inner[17], outer[17];
    u8 outer_color[4], inner_color[4];
    s32 angle, field_11C;
} Variant405EntryCurtain;

typedef struct {
    SVECTOR inner[8], outer[8];
    u8 outer_color[4], inner_color[4];
    s32 angle, field_8C;
} Variant405EntrySpoke;

typedef struct {
    u8 unknown_00[0x48];
    u8 outer[4], inner[4];
    s32 field_50, field_54, field_58;
    u8 unknown_5C[0x28];
} Variant405EntryRibbon;

typedef struct {
    u8 unknown_000[0x81A];
    s16 angle;
    u8 unknown_81C[0x200];
} Variant405EntryTrail;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant405EntryRing rings[3];
    Variant405EntrySheet sheets[6];
    Variant405EntryCurtain outer_curtains[3];
    Variant405EntryCurtain inner_curtains[3];
    Variant405EntrySpoke spokes[4];
    Variant405EntryRibbon ribbons[2];
    Variant405EntryTrail trails[3];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 glyphs[16];
    POLY_FT4 flat_textured[2];
    u8 unknown_3198[0x24];
    MATRIX matrix;
    SVECTOR target;
    SVECTOR origin;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    u32 frame;
    u32 animation_frame;
    s32 step, fade, field_322C;
    Variant405EntryConfig *G32 config;
    u8 unknown_3234[4];
    GsCOORDUNIT *G32 parts[1];
    s32 field_323C, field_3240, field_3244, field_3248, field_324C;
    u8 unknown_3250[4];
    s32 phase, field_3258;
    u8 unknown_325C[8];
    s32 tint;
    s16 slot, command;
} Variant405EntryState;

typedef struct {
    u8 unknown_00[60];
    u16 texture_cluts[2];
} Variant405EntryStack;

extern u8 gSpanishModelVariant405_D1FBC[];
void SpanishModelVariant405_DrawPulses(u8 *context);
void FrenchModelVariant562_DrawShards(u8 *context);
void SpanishModelVariant405_DrawOuterBands(u8 *context);
typedef char Variant405EntryConfigSize[(sizeof(Variant405EntryConfig) == 0x20) ? 1 : -1];
typedef char Variant405EntryStateSize[(sizeof(Variant405EntryState) == 0x326C) ? 1 : -1];
#endif
