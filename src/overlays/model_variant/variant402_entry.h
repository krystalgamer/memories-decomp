#ifndef MEMORIES_MODEL_VARIANT402_ENTRY_H
#define MEMORIES_MODEL_VARIANT402_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant439_entry.h"

/* One 0x34-byte row of header 402's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u16 spin;
    u8 unknown_1E[2];
    u32 start;
    u32 limit;
    u32 follow_limit;
    u8 unknown_2C[8];
} Variant402EntryConfig;

/* The swarm helper's thirty-two quads, laid out like the quad group. */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32];
    SVECTOR positions[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 size[32], done[32], reset[32];
} Variant402EntrySwarm;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantQuadGroup quads[1];
    Variant402EntrySwarm swarm;
    ModelVariantTrail streaks[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2], glow;
    u8 unknown_2244[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant402EntryConfig *config;
    u32 field_22D4;
    GsCOORDUNIT *parts[3];
    s16 field_22E4, field_22E6, field_22E8, field_22EA;
    s32 field_22EC, field_22F0;
    ModelVariantEntryTail tail;
} Variant402EntryState;

extern GsIMAGE gModelVariant402_aImages[9];
extern Variant402EntryConfig gModelVariant402_aConfigs[];
void FrenchModelVariant419_DrawSwarm(Variant402EntryState *context);
void FrenchModelVariant419_DrawQuads(Variant402EntryState *context);
#endif
