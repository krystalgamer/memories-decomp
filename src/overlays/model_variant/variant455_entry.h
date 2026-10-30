#ifndef MODEL_VARIANT455_ENTRY_H
#define MODEL_VARIANT455_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_effect_state.h"

/* One 0x34-byte command record of the table at 0x8013DD30. */
typedef struct {
    CVECTOR inner, outer;
    u8 unknown_08[8];
    u8 parts[3];
    u8 unknown_13[9];
    u32 fan_start, mesh_start, bloom_start;
    u8 unknown_28[0x0C];
} EntryConfig455;

/* One 0x2B4-byte group of twelve blooming quads (drawn by func_8013C110). */
typedef struct {
    SVECTOR a[12], b[12], c[12], d[12];
    u8 unknown_180[0x60];
    CVECTOR color;
    u8 unknown_1E4[0x10];
    s32 size[12], done[12], field_254[12];
    VECTOR origin, point, direction;
} EntryBloom455;

/* A 0x954-byte two-layer disc of seven 17-point rings. */
typedef struct {
    SVECTOR upper[7][17];
    SVECTOR lower[7][17];
    CVECTOR colors[7][17];
    s32 progress, state;
} EntryDisc455;

/* One 0x104-byte fan with four graded outer and inner colours. */
typedef struct {
    u8 unknown_00[0xA0];
    CVECTOR outer[4];
    CVECTOR inner[4];
    u8 unknown_C0[0x44];
} EntryFan455;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection455;

typedef struct {
    EntryBloom455 blooms[8];
    EntryDisc455 discs[1];
    EntryFan455 fans[12];
    ModelVariantSheet sheets[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_F4 mask;
    POLY_GT4 textured[2];
    POLY_GT4 glow[2], beam[2], flare[2];
    u8 unknown_2DB4[0x34];
    POLY_FT4 flat[2], ring_packet;
    u8 unknown_2E60[0x24];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig455 *G32 config;
    u32 field_2F00;
    GsCOORDUNIT *G32 parts[3];
    s16 field_2F10, field_2F12, field_2F14, field_2F16;
    s32 field_2F18, field_2F1C, field_2F20, field_2F24, field_2F28, field_2F2C, field_2F30;
    SVECTOR camera_offset;
    s32 ambient, phase;
    s16 slot, command;
} EntryState455;

/* Nine-entry GsIMAGE table (entries 1, 3, 5 and 7 are uploaded) and the
 * command table; the French image places them at 0x8013DC58/0x8013DD54. */
extern GsIMAGE D_8013DC34[9];
extern EntryConfig455 D_8013DD30[];
void func_8013C110(u8 *context);
void func_8013C738(u8 *context);
void func_8013CD3C(u8 *context);
void func_8013D888(u8 *context);

#endif
