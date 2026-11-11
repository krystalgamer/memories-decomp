#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

/* One 0x24-byte command record of the table at gSpanishModelVariant388_aConfigs. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u32 sheet_start, grow_end;
    u8 unknown_18[4];
    u32 track_end, shrink_end;
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

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant388EntryTube tubes[1];
    Variant388EntrySheet sheets[2];
    u8 unknown_228[0x3A0];
    Variant388EntryFeedback feedback[5];
    ModelVariantColorRing spokes[4];
    ModelVariantSpiralWide arms[12];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 paged;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_17E0[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant388EntryConfig *config;
    u8 unknown_1870[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    s16 spiral_angle, field_1886, spiral_size, spiral_shrink;
    s16 field_188C, field_188E, progress, strip_width;
    s32 ribbon_angle;
    s32 phase;
    s32 field_189C;
    u8 unknown_18A0[8];
    s32 tint;
    s16 slot, command;
} Variant388EntryState;

extern GsIMAGE gSpanishModelVariant388_aImages[9];
extern Variant388EntryConfig gSpanishModelVariant388_aConfigs[];
void FrenchModelVariant388_DrawStrips(u8 *context);
void SpanishModelVariant388_DrawRibbonFan(u8 *context);
void SpanishModelVariant388_DrawSheets(u8 *context);
void SpanishModelVariant388_DrawFeedbackStrips(u8 *context);
void FrenchModelVariant388_DrawPetals(u8 *context);
typedef char Variant388EntryConfigSize[(sizeof(Variant388EntryConfig) == 0x24) ? 1 : -1];
typedef char Variant388EntryStateSize[(sizeof(Variant388EntryState) == 0x18B0) ? 1 : -1];
#endif
