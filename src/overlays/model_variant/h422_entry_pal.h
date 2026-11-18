#ifndef MEMORIES_FRENCH_MODEL_VARIANT439_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT439_ENTRY_H
#include "../../types.h"
#include "e503_h320_entry_pal.h"

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantTwoToneScreenRing screen_rings[3];
    ModelVariantCurtain curtains[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_18C0[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_1950;
    GsCOORDUNIT *parts[3];
    s16 field_1960, field_1962, scale, field_1966;
    s32 progress, field_196C;
    CVECTOR outer, inner;
    s32 curtain_angle;
    ModelVariantEntryTail tail;
} Variant439EntryState;

extern GsIMAGE gFrenchModelVariant439_aImages[9];
extern ModelVariantWebConfig gFrenchModelVariant439_aConfigs[];
void ModelVariant422_DrawBands(Variant439EntryState *context);
void ModelVariant422_DrawSheets(Variant439EntryState *context);
void ModelVariant422_DrawCurtains(Variant439EntryState *context);
#endif
