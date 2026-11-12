#ifndef MEMORIES_MODEL_VARIANT416_ENTRY_H
#define MEMORIES_MODEL_VARIANT416_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    s32 field_70[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_98, field_9A;
    s32 field_9C;
    u8 unknown_A0[8];
} Variant416EntryBand;

typedef struct {
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    Variant416EntryBand bands[2];
    ModelVariantSheet sheets[3];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_19D4[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_1A64;
    GsCOORDUNIT *parts[3];
    s16 field_1A74, field_1A76, field_1A78, field_1A7A;
    s32 field_1A7C, field_1A80;
    ModelVariantEntryTail tail;
} Variant416EntryState;

extern GsIMAGE gModelVariant416_aImages[9];
extern ModelVariantWebConfig gModelVariant416_aConfigs[];
void ModelVariant416_DrawBands(u8 *context);
void ModelVariant416_DrawSheets(u8 *context);
void ModelVariant416_DrawWebs(u8 *context);
void FrenchModelVariant433_DrawRays(u8 *context);
#endif
