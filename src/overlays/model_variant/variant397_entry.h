#ifndef MEMORIES_MODEL_VARIANT397_ENTRY_H
#define MEMORIES_MODEL_VARIANT397_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u32 linked;
    u8 parts[3], unknown_17[9];
    u32 start, progress_start, progress_end, shrink_start, shrink_end;
    s32 offset[3];
    u16 extent, field_42;
    s32 radius;
} Variant397EntryConfig;

typedef struct {
    SVECTOR a[5], b[5], c[5];
    long sa[5], sb[5], sc[5];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0;
    s32 otz[5];
    long flag[5];
} Variant397EntryBand;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant397EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E88[0x24];
    MATRIX matrix, matrix0, matrix1;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant397EntryConfig *config;
    u32 field_F58;
    GsCOORDUNIT *parts[3];
    s16 field_F68, field_F6A, scale, field_F6E;
    s32 progress, field_F74;
    ModelVariantEntryTail tail;
} Variant397EntryState;

extern GsIMAGE gModelVariant397_aImages[9];
extern Variant397EntryConfig gModelVariant397_aConfigs[];
void ModelVariant397_DrawBands(u8 *context);
void ModelVariant397_DrawSheets(u8 *context);
void ModelVariant397_DrawWebs(u8 *context);
#endif
