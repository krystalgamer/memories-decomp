#ifndef MEMORIES_FRENCH_MODEL_VARIANT464_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT464_ENTRY_H
#include "../../types.h"
#include "e503_h320_entry_pal.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 grow_end;
    u8 unknown_24[4];
    u32 fade_begin, fade_end;
    u32 times[4];
    s32 count;
    s32 group_count;
} FrenchVariant464EntryConfig;

/* One advancing quad pair: six points (three per quad), their projections,
 * inner and outer colours, the scale, per-quad depth and staggered progress. */
typedef struct {
    SVECTOR points[6];
    long projected[3][2];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_68, field_6A;
    s32 field_6C;
    s32 depth[2];
    s32 progress[2];
    s32 active;
} FrenchVariant464EntryPrimary;

typedef struct {
    ModelVariantWebNarrow webs[3];
    FrenchVariant464EntryPrimary primary[5];
    ModelVariantSheetSet sheets[6];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan fans[1];
    ModelVariantScreenRing screen_rings[3];
    ModelVariantCurtain curtains[3];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], ring_poly, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_1BA0[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    FrenchVariant464EntryConfig *config;
    u32 field_1C30;
    GsCOORDUNIT *parts[3];
    s32 timing_index;
    s16 field_1C44, field_1C46, width, field_1C4A;
    s32 field_1C4C, field_1C50;
    CVECTOR outer, inner;
    s32 curtain_angle;
    ModelVariantEntryTail tail;
} FrenchVariant464EntryState;

extern GsIMAGE gFrenchModelVariant464_aImages[9];
extern FrenchVariant464EntryConfig gFrenchModelVariant464_aConfigs[];
void FrenchModelVariant464_DrawRings(FrenchVariant464EntryState *context);
void SpanishModelVariant464_DrawAdvancingQuads(FrenchVariant464EntryState *context);
void SpanishModelVariant464_DrawSheets(FrenchVariant464EntryState *context);
#endif
