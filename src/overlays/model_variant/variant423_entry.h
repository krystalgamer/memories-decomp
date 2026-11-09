#ifndef MEMORIES_MODEL_VARIANT423_ENTRY_H
#define MEMORIES_MODEL_VARIANT423_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    u8 unknown_00[0xB4];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0;
    u8 unknown_F4[0x14];
} Variant423EntryBand;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant423EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E74[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_F04;
    GsCOORDUNIT *parts[3];
    s16 field_F14, field_F16, field_F18, field_F1A, field_F1C, field_F1E;
    s32 field_F20;
    ModelVariantEntryTail tail;
} Variant423EntryState;

extern GsIMAGE gModelVariant423_aImages[9];
extern ModelVariantWebConfig gModelVariant423_aConfigs[];
void ModelVariant423_DrawBand(u8 *context);
void ModelVariant423_DrawSheets(u8 *context);
void ModelVariant423_DrawWebs(u8 *context);
#endif
