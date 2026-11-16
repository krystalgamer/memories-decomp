#ifndef MEMORIES_MODEL_VARIANT416_ENTRY_H
#define MEMORIES_MODEL_VARIANT416_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0xA8-byte band of two segments: three rows of two points and their
 * screen coordinates, per segment the growth level (staggered below zero by
 * the entry, grown to 0x400 by the band drawer) and done flag, the two colour
 * rows and scale the entry seeds, and the two depths. */
typedef struct {
    SVECTOR a[2], b[2], c[2];
    long sa[2], sb[2], sc[2];
    u8 unknown_48[0x20];
    s32 level[2];
    s32 done[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_98, field_9A;
    s32 field_9C;
    s32 otz[2];
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
    u8 unknown_19D4[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_1A64;
    GsCOORDUNIT *parts[3];
    s16 field_1A74, field_1A76, scale, field_1A7A;
    s32 field_1A7C, quad_angle;
    ModelVariantEntryTail tail;
} Variant416EntryState;

extern GsIMAGE gModelVariant416_aImages[9];
extern ModelVariantWebConfig gModelVariant416_aConfigs[];
void ModelVariant416_DrawBands(u8 *context);
void ModelVariant416_DrawSheets(u8 *context);
void ModelVariant416_DrawWebs(u8 *context);
void FrenchModelVariant433_DrawRays(u8 *context);
#endif
