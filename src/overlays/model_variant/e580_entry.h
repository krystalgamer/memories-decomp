#ifndef MEMORIES_MODEL_VARIANT397_ENTRY_H
#define MEMORIES_MODEL_VARIANT397_ENTRY_H
#include "../../types.h"
#include "../model_variant/e503_h320_entry_pal.h"
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
    ModelVariantWebNarrow webs[3];
    ModelVariantBandShort bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E88[0x10];
    GsGLINE line;
    MATRIX matrix, matrix0, matrix1;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant397EntryConfig *config;
    u32 field_F58;
    GsCOORDUNIT *parts[3];
    s16 field_F68, field_F6A, scale, field_F6E;
    s32 progress, quad_angle;
    ModelVariantEntryTail tail;
} Variant397EntryState;

extern GsIMAGE gModelVariant397_aImages[9];
extern Variant397EntryConfig gModelVariant397_aConfigs[];
void ModelVariant397_DrawBands(Variant397EntryState *context);
void ModelVariant397_DrawSheets(Variant397EntryState *context);
void ModelVariant397_DrawWebs(Variant397EntryState *context);
#endif
