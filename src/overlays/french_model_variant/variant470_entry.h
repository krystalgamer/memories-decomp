#ifndef FRENCH_MODEL_VARIANT470_ENTRY_H
#define FRENCH_MODEL_VARIANT470_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    CVECTOR color;
    u8 unknown_194[0x10];
    s32 size[10], done[10], field_1F4[10], initialized[10];
    MATRIX matrices[10];
    VECTOR captured[10], directions[10];
} EntryPair470;

typedef struct {
    EntryPair470 pairs[2];
    ModelVariantQuadGroup groups[1];
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantScaledSheet sheets[2];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2], single_packet, pair_packet;
    GsGLINE line;
    u8 unknown_2574[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    ModelVariantMoveConfig *config;
    u32 field_25F0;
    GsCOORDUNIT *parts[2];
    u32 field_25FC;
    s16 field_2600, field_2602;
    u32 motion;
    s16 field_2608, field_260A;
    s32 field_260C;
    SVECTOR camera_offset;
    s32 ambient, field_261C, phase;
    s16 slot, command;
} EntryState470;

extern GsIMAGE gFrenchModelVariant470_aImages[9];
extern ModelVariantMoveConfig gFrenchModelVariant470_aConfigs[];
void FrenchModelVariant470_DrawQuadPairs(u8 *context);
void SpanishModelVariant470_DrawQuads(u8 *context);

#endif
