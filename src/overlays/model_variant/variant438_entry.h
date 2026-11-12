#ifndef MEMORIES_MODEL_VARIANT438_ENTRY_H
#define MEMORIES_MODEL_VARIANT438_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x44-byte row of header 438's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 count;
    s32 mode;
    u32 rate;
    u32 start;
    u32 limit;
    u8 unknown_30[8];
    u32 track_end, sweep_start, sweep_end;
} Variant438EntryConfig;

/* Up to sixty-four swarm quads at the start of the work area; the swarm
 * helper animates them from the part matrices captured at launch. */
typedef struct {
    SVECTOR a[64], b[64], c[64], d[64], origin[64];
    u8 color[4];
    u8 unknown_A04[0x10];
    s32 size[64];
    s32 field_B14[64];
    s32 field_C14[64];
    s32 done[64];
    MATRIX origins[64];
} Variant438EntrySwarm;

typedef struct {
    Variant438EntrySwarm swarm;
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
    u8 unknown_2EC8[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[64];
    VECTOR offsets[64];
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant438EntryConfig *config;
    u32 field_3758;
    GsCOORDUNIT *parts[3];
    s16 field_3768, field_376A;
    s32 sweep;
    s16 field_3770, field_3772;
    s32 field_3774, field_3778;
    ModelVariantEntryTail tail;
} Variant438EntryState;

extern GsIMAGE gModelVariant438_aImages[9];
extern Variant438EntryConfig gModelVariant438_aConfigs[];
void FrenchModelVariant455_DrawSwarm(u8 *context);
#endif
