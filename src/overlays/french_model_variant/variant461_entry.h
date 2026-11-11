#ifndef FRENCH_MODEL_VARIANT461_ENTRY_H
#define FRENCH_MODEL_VARIANT461_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    ModelVariantCountedRibbon ribbons[5];
    ModelVariantQuadRing ring;
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheetSet quads[10];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 unknown_2E34[0x34];
    POLY_FT4 flat[2], ring_packet;
    GsGLINE line;
    u8 unknown_2EF4[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR origin, path_origins[5], path_targets[5], paths[5], direction;
    DVECTOR screen_delta;
    VECTOR ring_positions[24], ring_directions[24], view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    ModelVariantGrowConfig *config;
    u32 field_3370;
    GsCOORDUNIT *parts[3];
    s32 angle, scale, wave, intensity, ribbon_wave, field_3394;
    s16 field_3398, field_339A;
    s32 field_339C;
    s16 field_33A0, field_33A2;
    s32 field_33A4, field_33A8, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState461;

extern GsIMAGE gFrenchModelVariant461_aImages[9];
extern ModelVariantGrowConfig gFrenchModelVariant461_aConfigs[];
void FrenchModelVariant461_DrawRibbons(u8 *context);
void SpanishModelVariant461_DrawSheets(u8 *context);
void SpanishModelVariant461_DrawRing(u8 *context);

#endif
