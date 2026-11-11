#ifndef MEMORIES_MODEL_VARIANT412_ENTRY_H
#define MEMORIES_MODEL_VARIANT412_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 limit;
    u8 unknown_24[0xC];
    u32 sweep_start, sweep_end;
    s32 height;
} Variant412EntryConfig;

/* Six sprites at work + 0x2FC, released from the opposite slot. */
typedef struct {
    SVECTOR v0[6], v1[6], v2[6], v3[6], origin[6];
    CVECTOR color;
    u8 unknown_F4[0x10];
    s32 scale[6];
    s32 field_11C[6];
    s32 field_134[6];
    s32 done[6];
} Variant412EntryFlares;

typedef struct {
    ModelVariantSpriteSet trails[1];
    Variant412EntryFlares flares;
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_1D14[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[6];
    VECTOR offsets[6];
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant412EntryConfig *config;
    u32 field_1E64;
    GsCOORDUNIT *parts[3];
    s16 field_1E74, field_1E76;
    s32 sweep;
    s16 field_1E7C, field_1E7E;
    s32 field_1E80, field_1E84;
    ModelVariantEntryTail tail;
} Variant412EntryState;

extern GsIMAGE gModelVariant412_aImages[9];
extern Variant412EntryConfig gModelVariant412_aConfigs[];
void FrenchModelVariant429_DrawSwarm(u8 *context);
void FrenchModelVariant429_DrawQuads(u8 *context);
#endif
