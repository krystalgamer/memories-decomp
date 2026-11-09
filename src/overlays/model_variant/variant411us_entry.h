#ifndef MEMORIES_MODEL_VARIANT411US_ENTRY_H
#define MEMORIES_MODEL_VARIANT411US_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* The 0x38-byte config: the header-412 config without its height word. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 limit;
    u8 unknown_24[0xC];
    u32 sweep_start, sweep_end;
} Variant411UsEntryConfig;

typedef struct {
    ModelVariantSpriteSet trails[1];
    ModelVariantSwarm swarm;
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
    u8 unknown_22C4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[32];
    VECTOR offsets[32];
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant411UsEntryConfig *config;
    u32 field_2754;
    GsCOORDUNIT *parts[3];
    s16 field_2764, field_2766;
    s32 sweep;
    s16 field_276C, field_276E;
    s32 field_2770, field_2774;
    ModelVariantEntryTail tail;
} Variant411UsEntryState;

extern GsIMAGE gModelVariant411_aImages[9];
extern Variant411UsEntryConfig gModelVariant411_aConfigs[];
void FrenchModelVariant428_DrawSwarm(u8 *context);
void FrenchModelVariant428_DrawQuads(u8 *context);
#endif
