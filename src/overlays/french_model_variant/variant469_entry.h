#ifndef FRENCH_MODEL_VARIANT469_ENTRY_H
#define FRENCH_MODEL_VARIANT469_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../french_model_variant/variant582_arcs.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    CVECTOR outer, inner, arc_color;
    u8 unknown_0C[4], parts[3], unknown_13[0x11];
    u32 draw_start, arc_limit;
    u8 unknown_2C[8];
    u32 matrix_end;
    u8 unknown_38[8];
} EntryConfig469;

typedef struct {
    SVECTOR points[3][13];
    CVECTOR colors[13];
    s32 progress, state;
} EntrySheet469;

typedef struct {
    Arcs582Group arcs[4];
    EntrySheet469 sheets[5];
    ModelVariantCountedRibbon ribbons[5];
    ModelVariantQuadRing ring;
    ModelVariantTrail trails[16];
    ModelVariantBandPair bands[1];
    ModelVariantSheetSet quads[10];
    ModelVariantColorRing rings_a[6], rings_b[4];
    u8 unknown_32A0[0x90];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_F4 mask;
    POLY_GT4 textured[2];
    u8 unknown_33F0[0x34];
    POLY_FT4 flat[2], ring_packet;
    GsGLINE line;
    u8 unknown_34B0[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    VECTOR target_center, positions[5], path_origins[5], path_targets[5], paths[5];
    VECTOR direction, line_direction;
    DVECTOR screen_delta;
    VECTOR ring_positions[24], ring_directions[24], view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig469 *config;
    u32 field_399C;
    GsCOORDUNIT *parts[3];
    s32 angle, scale, wave, intensity, field_39BC, field_39C0;
    s16 field_39C4, field_39C6;
    s32 field_39C8;
    s16 field_39CC, field_39CE;
    s32 field_39D0, field_39D4, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState469;

extern GsIMAGE gFrenchModelVariant469_aImages[9];
extern EntryConfig469 gFrenchModelVariant469_aConfigs[];
void FrenchModelVariant582_DrawArcs(u8 *context);
void FrenchModelVariant582_DrawSheets(u8 *context);
void SpanishModelVariant469_DrawQuadRing(u8 *context);

#endif
