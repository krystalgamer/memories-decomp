#ifndef FRENCH_MODEL_VARIANT477_ENTRY_H
#define FRENCH_MODEL_VARIANT477_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    ModelVariantPanel panels[2];
    ModelVariantQuadGroup groups[2];
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantScaledSheet sheets[2];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat[2], quads_packet, panels_packet;
    GsGLINE line;
    u8 unknown_2000[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
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
