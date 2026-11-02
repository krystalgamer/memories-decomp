#ifndef MEMORIES_FRENCH_MODEL_VARIANT464_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT464_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
    u32 times[4];
    s32 count;
    u8 unknown_44[4];
} Variant464EntryConfig;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_68, field_6A;
    s32 field_6C;
    u8 unknown_70[8];
    s32 progress[2];
    s32 active;
} Variant464EntryPrimary;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant464EntryFan;

typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR color[17];
    s32 scale, count;
} Variant464EntryScreenRing;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant464EntryPrimary primary[5];
    ModelVariantSheetSet sheets[6];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant464EntryFan fans[1];
    Variant464EntryScreenRing screen_rings[3];
    ModelVariantCurtain curtains[3];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], unknown_1AE8, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_1BA0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant464EntryConfig *G32 config;
    u32 field_1C30;
    GsCOORDUNIT *G32 parts[3];
    s32 timing_index;
    s16 field_1C44, field_1C46, field_1C48, field_1C4A;
    s32 field_1C4C, field_1C50;
    CVECTOR outer, inner;
    s32 field_1C5C, phase;
    u8 unknown_1C64[8];
    s32 tint;
    s16 slot, command;
} Variant464EntryState;

extern u8 gFrenchModelVariant464_D2DAC[];
void FrenchModelVariant464_DrawRings(u8 *context);
void SpanishModelVariant464_DrawAdvancingQuads(u8 *context);
void SpanishModelVariant464_DrawSheets(u8 *context);
#endif
