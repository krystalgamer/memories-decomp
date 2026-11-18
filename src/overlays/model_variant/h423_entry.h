#ifndef MEMORIES_MODEL_VARIANT423_ENTRY_H
#define MEMORIES_MODEL_VARIANT423_ENTRY_H
#include "../../types.h"
#include "../model_variant/h320_entry_pal.h"

/* One 0x108-byte band: three rows of five points and their screen
 * coordinates, the two colour rows and scale the entry seeds, and the five
 * depths. */
typedef struct {
    SVECTOR a[5], b[5], c[5];
    long sa[5], sb[5], sc[5];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0;
    s32 otz[5];
} Variant423EntryBand;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant423EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E74[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_F04;
    GsCOORDUNIT *parts[3];
    s16 field_F14, field_F16, scale, field_F1A, progress, band_wave;
    s32 quad_angle;
    ModelVariantEntryTail tail;
} Variant423EntryState;

extern GsIMAGE gModelVariant423_aImages[9];
extern ModelVariantWebConfig gModelVariant423_aConfigs[];
void ModelVariant423_DrawBand(Variant423EntryState *context);
void ModelVariant423_DrawSheets(Variant423EntryState *context);
void ModelVariant423_DrawWebs(Variant423EntryState *context);
#endif
