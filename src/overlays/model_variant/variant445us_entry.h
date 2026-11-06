#ifndef MEMORIES_DECOMP_MODEL_VARIANT445US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT445US_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_graphics_state.h"

typedef struct {
    CVECTOR outer, inner, web_color;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 first_count, paired;
    u32 draw_start, grow_end, field_2C, phase3_end, matrix_end;
    u8 unknown_38[8];
} EntryConfig445Us;

/* One 0x2B0-byte ribbon: a thirteen-point spine and a copy of it moved along
 * the view, both projected, per point the screen angle and projected width,
 * the ribbon colour, the drawn length, the phase state, the view offset, the
 * depth and the width's screen offset. */
typedef struct {
    SVECTOR a[13];
    long sa[13];
    s32 angle[13];
    SVECTOR b[13];
    long sb[13];
    s32 width[13];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_1B8, field_1BC, count, field_1C4, state, length;
    u8 unknown_1D0[0x68];
    VECTOR position;
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} EntryRibbon445Us;

typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
    s32 field_98;
} EntryQuad445Us;

typedef struct {
    EntryRibbon445Us ribbons[5];
    ModelVariantQuadRing ring;
    ModelVariantTrail trails[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    EntryQuad445Us quads[10];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantShape shapes[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 unknown_3014[0x34];
    POLY_FT4 flat[2], ring_packet;
    GsGLINE line;
    u8 unknown_30D4[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR origin, path_origins[5], path_targets[5], paths[5], direction;
    DVECTOR screen_delta;
    VECTOR ring_positions[24], ring_directions[24], view_delta;
    s32 angles[2];
    s32 frame_count;
    u32 elapsed, animation;
    s32 step, fade;
    EntryConfig445Us *config;
    u32 field_3550;
    GsCOORDUNIT *parts[3];
    s32 angle, scale, wave, intensity, ribbon_wave, ribbon_phase;
    s16 field_3578, field_357A;
    s32 field_357C;
    s16 field_3580, field_3582;
    s32 field_3584, field_3588, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState445Us;

extern u8 gModelVariant445_D2848[];
void ModelVariant445_DrawRibbons(u8 *context);
void FrenchModelVariant462_DrawSheets(u8 *context);
void FrenchModelVariant462_DrawRings(u8 *context);

#endif
