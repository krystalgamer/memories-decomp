#ifndef MEMORIES_DECOMP_MODEL_VARIANT460_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT460_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/camera_view.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"

/* Header 460's 0x20BC-byte loader work area. */
typedef struct {
    ModelVariantPanel panels[2];
    ModelVariantQuadGroup groups[2];
    ModelVariantTrail columns[16];
    ModelVariantWebNarrow domes[3];
    ModelVariantBandPair pair;
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing halos[4];
    ModelVariantShape star;
    ModelVariantPackets packets;
    POLY_GT4 glow;
    POLY_FT4 flat[2];
    POLY_FT4 quads_packet;
    POLY_FT4 panels_packet;
    u8 unknown_1FEC[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantMoveConfig *config;
    u8 unknown_207C[4];
    GsCOORDUNIT *parts[2];
    u8 unknown_2088[4];
    s16 field_208C, field_208E;
    s32 field_2090;
    s16 field_2094, field_2096;
    s32 field_2098;
    s32 enabled[2];
    s32 field_20A4;
    s32 phase;
    SVECTOR orbit;
    s32 tint;
    s16 slot, command;
} Variant460State;

extern GsIMAGE gModelVariant460_aImages[9];
extern ModelVariantMoveConfig gModelVariant460_aConfigs[];
s32 ModelVariant460_HandleRequest(u8 *context, s32 command);
void SpanishModelVariant477_DrawPanels(Variant460State *context);
void SpanishModelVariant477_DrawQuadGroups(Variant460State *context);
#endif
