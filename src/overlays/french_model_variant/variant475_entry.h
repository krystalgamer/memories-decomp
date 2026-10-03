#ifndef MEMORIES_FRENCH_MODEL_VARIANT475_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT475_ENTRY_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../model_variant/variant458_streamers.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4], parts[8], unknown_0C[0x14];
    u32 start, field_24, orbit_start, ribbon_start, phase4, phase5;
} Variant475EntryConfig;

typedef struct {
    SVECTOR a[8], b[8], c[8], d[8];
    u8 unknown_100[0x40];
    CVECTOR color;
    u8 unknown_144[0x10];
    s32 level[8], hidden[8], field_194[8];
    VECTOR position;
    u8 unknown_1C4[0x20];
} Variant475EntryQuads;

typedef struct {
    SVECTOR a[13];
    PSXLONG sa[13];
    s32 angle[13];
    SVECTOR b[13];
    PSXLONG sb[13];
    s32 width[13];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_1B8, field_1BC, count, field_1C4, state, len;
    u8 unknown_1D0[0x68];
    VECTOR velocity;
    s32 otz[13];
    PSXLONG flag[13];
    s16 ox[13], oy[13];
} Variant475EntryRibbon;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant475EntryQuads quads[1];
    Variant475EntryRibbon ribbons[8];
    ModelVariantSheetSet sheets[16];
    u8 unknown_22C4[0x318];
    Variant458Streamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat[2];
    u8 unknown_2D70[0x28];
    POLY_FT4 extra_flat;
    u8 unknown_2DC0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR starts[8], ends[8], deltas[8], direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant475EntryConfig *G32 config;
    u32 unknown_2FD0;
    GsCOORDUNIT *G32 parts[8];
    s16 field_2FF4, field_2FF6, field_2FF8, field_2FFA;
    s32 field_2FFC, orbit, orbit_progress, field_3008;
    s32 field_300C, field_3010, field_3014, field_3018, field_301C, phase;
    u8 unknown_3024[8];
    s32 tint;
    s16 slot, command;
} Variant475EntryState;

typedef struct {
    PSXLONG origin;
    PSXLONG p, flag;
    PSXLONG target;
} Variant475EntryProjection;

extern u8 D_8013DD38[];
void func_8013BE98(u8 *context);
void func_8013C978(u8 *context);
void func_8013CE34(u8 *context);

typedef char Variant475EntryConfigSize[(sizeof(Variant475EntryConfig) == 56) ? 1 : -1];
typedef char Variant475EntryQuadsSize[(sizeof(Variant475EntryQuads) == 0x1E4) ? 1 : -1];
typedef char Variant475EntryRibbonSize[(sizeof(Variant475EntryRibbon) == 0x2E4) ? 1 : -1];
typedef char Variant475EntryStateSize[(sizeof(Variant475EntryState) == 0x3034) ? 1 : -1];
#endif
