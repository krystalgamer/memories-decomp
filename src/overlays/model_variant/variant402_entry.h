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

/* Ten growing quads launched along the target direction; the quads helper
 * animates them. */
typedef struct {
    SVECTOR a[10], b[10], c[10], d[10];
    SVECTOR position[10];
    u8 color[4];
    s32 size[10], angle[10], done[10];
    u8 unknown_20C[0x28];
    s32 reset[10];
    u8 unknown_25C[0xA0];
} Variant402EntryQuads;

/* The swarm helper's thirty-two quads, laid out like the quad group. */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32];
    SVECTOR position[32];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32], done[32], reset[32];
} Variant402EntrySwarm;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant402EntryQuads quads[1];
    Variant402EntrySwarm swarm;
    ModelVariantTrail streaks[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], glow;
    u8 unknown_2244[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
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
void FrenchModelVariant419_DrawSwarm(u8 *context);
void FrenchModelVariant419_DrawQuads(u8 *context);
#endif
