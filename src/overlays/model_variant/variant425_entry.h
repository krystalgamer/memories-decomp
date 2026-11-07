#ifndef MEMORIES_MODEL_VARIANT425_ENTRY_H
#define MEMORIES_MODEL_VARIANT425_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* The entry's view of the ray record (the same layout as Variant425Ray),
 * with the three colour rows the entry fills named; its arm records are
 * ModelVariantSpiralArm. */
typedef struct {
    u8 unknown00[0x18];
    SVECTOR a[3];
    long sa[3];
    s32 angle[3];
    SVECTOR b[3];
    long sb[3];
    s32 width[3];
    u8 cb[3][4];
    u8 ca[3][4];
    u8 unknown90[4];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    u8 unknownAC[0xC];
} Variant425EntryRay;

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x18];
} Variant425EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebSmall webs[3];
    ModelVariantSpiralArm arms[16];
    Variant425EntryRay rays[16];
    u8 unknown_1940[0x1CA0 - 0x1940];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_265C[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR orbit;
    VECTOR direction;
    VECTOR offset;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_2708, field_270A, field_270C, field_270E;
    s32 field_2710;
    Variant425EntryConfig *config;
    u32 field_2718;
    GsCOORDUNIT *parts[3];
    s16 field_2728, field_272A, field_272C, field_272E;
    s32 field_2730, sweep, field_2738, field_273C, field_2740, field_2744;
    s32 phase, field_274C;
    u8 unknown_2750[8];
    s32 tint;
    s16 slot, command;
} Variant425EntryState;

extern GsIMAGE gModelVariant425_aImages[9];
extern Variant425EntryConfig gModelVariant425_aConfigs[];
void ModelVariant425_DrawSheet(u8 *context);
void ModelVariant425_DrawSpiral(u8 *context);
void ModelVariant425_DrawRays(u8 *context);
void ModelVariant425_DrawWebs(u8 *context);
#endif
