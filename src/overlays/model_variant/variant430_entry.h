#ifndef MEMORIES_MODEL_VARIANT430_ENTRY_H
#define MEMORIES_MODEL_VARIANT430_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x34-byte row of header 430's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, end;
    u32 limit;
    u32 track_end;
    u8 unknown_2C[8];
} Variant430EntryConfig;

/* Thirty-two swarm quads at the start of the work area, drawn by
 * FrenchModelVariant447_DrawSwarm. */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32], origin[32];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32];
    s32 field_594[32];
    s32 field_614[32];
} Variant430EntrySwarm;

typedef struct {
    Variant430EntrySwarm swarm;
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
    u8 unknown_1F48[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant430EntryConfig *config;
    u32 field_1FD8;
    GsCOORDUNIT *parts[3];
    s16 field_1FE8, field_1FEA;
    s16 field_1FEC, field_1FEE;
    s32 field_1FF0, field_1FF4;
    ModelVariantEntryTail tail;
} Variant430EntryState;

/* Nine texture images followed by the command table. */
extern GsIMAGE gModelVariant430_aImages[];
extern Variant430EntryConfig gModelVariant430_aConfigs[];
void FrenchModelVariant447_DrawSwarm(u8 *context);
#endif
