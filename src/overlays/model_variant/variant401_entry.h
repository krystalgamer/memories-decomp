#ifndef MEMORIES_MODEL_VARIANT401_ENTRY_H
#define MEMORIES_MODEL_VARIANT401_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    SVECTOR point[3][9];
    long screen[3][9];
    u8 unknown_144[0x90];
    s32 offset[9];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_250, field_252;
    s32 field_254;
    s32 otz[9];
} Variant401EntryBand;

typedef struct {
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    Variant401EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_flat_end[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_1A64;
    GsCOORDUNIT *parts[3];
    s16 field_1A74, field_1A76, scale, field_1A7A;
    s32 field_1A7C, field_1A80;
    ModelVariantEntryTail tail;
} Variant401EntryState;

extern GsIMAGE gModelVariant401_aImages[9];
extern ModelVariantWebConfig gModelVariant401_aConfigs[];
void ModelVariant401_DrawBand(u8 *context);
void FrenchModelVariant418_DrawSheets(u8 *context);
void ModelVariant416_DrawWebs(u8 *context);
void FrenchModelVariant433_DrawRays(u8 *context);
#endif
