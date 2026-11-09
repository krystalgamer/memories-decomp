#ifndef MEMORIES_MODEL_VARIANT413_ENTRY_H
#define MEMORIES_MODEL_VARIANT413_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant412_entry.h"

typedef struct {
    ModelVariantSwarm groups[3];
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_2DF0[0x24];
    MATRIX matrix;
    MATRIX origins[3];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[3][32];
    VECTOR offsets[3][32];
    VECTOR view_delta;
    ModelVariantClock clock;
    ModelVariantSweepConfig *config;
    u32 field_3AE0;
    GsCOORDUNIT *parts[3];
    s16 field_3AF0, field_3AF2;
    s32 sweep;
    s16 field_3AF8, field_3AFA;
    s32 field_3AFC, field_3B00;
    ModelVariantEntryTail tail;
} Variant413EntryState;

extern GsIMAGE gModelVariant413_aImages[9];
extern ModelVariantSweepConfig gModelVariant413_aConfigs[];
void FrenchModelVariant430_DrawSwarm(u8 *context);
#endif
