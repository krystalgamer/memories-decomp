#ifndef FRENCH_MODEL_VARIANT477_ENTRY_H
#define FRENCH_MODEL_VARIANT477_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    SVECTOR a[1], b[1], c[1], d[1], positions[1];
    CVECTOR color;
    u8 unknown_2C[0x10];
    s32 size[1], done[1], field_44[1], initialized[1];
    MATRIX matrices[1];
    VECTOR captured[1], directions[1];
} EntryPanel477;

typedef struct {
    EntryPanel477 panels[2];
    ModelVariantQuadGroup groups[2];
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantScaledSheet sheets[2];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2], quads_packet, panels_packet;
    GsGLINE line;
    u8 unknown_2000[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    ModelVariantMoveConfig *config;
    u32 field_207C;
    GsCOORDUNIT *parts[2];
    u32 field_2088;
    s16 field_208C, field_208E;
    u32 field_2090;
    s16 field_2094, field_2096;
    s32 field_2098, enabled[2], field_20A4, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState477;

extern GsIMAGE gFrenchModelVariant477_aImages[9];
extern ModelVariantMoveConfig gFrenchModelVariant477_aConfigs[];
void SpanishModelVariant477_DrawPanels(u8 *context);
void SpanishModelVariant477_DrawQuadGroups(u8 *context);

#endif
