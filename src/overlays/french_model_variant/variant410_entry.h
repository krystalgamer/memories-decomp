#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT410_ENTRY_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT410_ENTRY_H
#include "../../types.h"
#include "../../overlays/model_variant/model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../psyq/libhmd.h"

/* One 0x10-byte command row: the model part and the end frame. */
typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 end;
} Variant410EntryConfig;

/* Entry views of the helper records; the helpers keep their own views. */
typedef struct {
    SVECTOR points[4][10];
    SVECTOR positions[10];
    CVECTOR color;
    s32 scale[10];
    s32 angle[10];
    s32 done[10];
    s32 field_20C[10];
    s32 field_234[10];
    u8 unknown_25C[0x2FC - 0x25C];
} Variant410EntryQuads;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    VECTOR velocity;
    u8 unknown_250[0x2D8 - 0x250];
    s32 progress[17];
} Variant410EntryRibbon;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale, field_8C, field_90;
    u8 unknown_94[4];
} Variant410EntryRing;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant410EntryQuads quads[1];
    Variant410EntryRibbon ribbons[9];
    Variant410EntryRing rings[2];
    POLY_FT4 quad;
    POLY_G3 triangle;
    POLY_G4 gradient;
    POLY_GT4 textured[2];
    u8 unknown_20F8[0x2110 - 0x20F8];
    POLY_FT4 flat_textured[2];
    u8 unknown_2160[0x2170 - 0x2160];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant410EntryConfig *G32 config;
    u32 field_21DC;
    GsCOORDUNIT *G32 part;
    s16 field_21E4, width;
    s32 field_21E8, field_21EC, phase;
    u8 unknown_21F4[8];
    s32 tint;
    s16 slot, command;
} Variant410EntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant410EntryProjection;

extern u8 gFrenchModelVariant410_D1D04[];
void FrenchModelVariant410_DrawQuads(u8 *context);
void FrenchModelVariant410_DrawRibbons(u8 *context);
s32 FrenchModelVariant410_HandleRequest(SVECTOR *point, s32 command);
#endif
