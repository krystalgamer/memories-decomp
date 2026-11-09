#ifndef MEMORIES_FRENCH_MODEL_VARIANT341_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT341_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"
#include "../model_variant/variant324_fan.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 substeps;
    u8 unknown_0E[2];
    u32 duration;
} Variant341EntryConfig;

/* Minimum entry-observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    ModelVariantColorBandPadded bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    Variant324Fan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_EA0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame, step, fade;
    Variant341EntryConfig *config;
    u32 field_F30;
    GsCOORDUNIT *parts[3];
    s16 substep, field_F42, field_F44, field_F46;
    s32 field_F48, field_F4C;
    ModelVariantEntryTail tail;
} Variant341EntryState;

extern GsIMAGE gFrenchModelVariant341_aImages[9];
extern Variant341EntryConfig gFrenchModelVariant341_aConfigs[];
void ModelVariant324_DrawWebs(u8 *context);
void ModelVariant324_DrawFan(u8 *context);
void ModelVariant324_DrawBand(u8 *context);
void ModelVariant324_DrawMovingRing(u8 *context);
s32 FrenchModelVariant341_HandleRequest(SVECTOR *point, s32 command);
#endif
