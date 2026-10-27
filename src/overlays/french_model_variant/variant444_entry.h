#ifndef MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H

#include "../../types.h"
#include "variant337_entry.h"
#include "../spanish_model_variant/variant411_funnels.h"

typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 sheet_start, tube_start, tube_end, fade_start, fade_end, end;
} Variant444EntryConfig;

typedef struct {
    u8 unknown_000[0x510];
    CVECTOR color[9];
    VECTOR scale;
    s16 field_544, field_546;
    s32 field_548;
} Variant444EntryTube;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR color[8], inner_color, outer_color;
    s32 angle, count;
} Variant444EntrySpoke;

typedef struct {
    u8 unknown_000[0xD8];
    CVECTOR inner, outer;
    VECTOR scale;
    u8 unknown_0F0[0x3C];
} Variant444EntryBeam;

typedef struct {
    u8 unknown_00[0x40];
    CVECTOR color_40, color_44;
    s32 field_48;
    u8 unknown_4C[0x18];
} Variant444EntryColorRecord;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant444EntryTube tubes[1];
    ModelVariantSheet sheets[2];
    Funnel411 funnels[2];
    Variant444EntrySpoke spokes[4];
    Variant444EntryBeam beams[2];
    Variant444EntryColorRecord records[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], plain, paged, extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1598[0x4C];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant444EntryConfig *G32 config;
    u32 field_1650;
    GsCOORDUNIT *G32 part;
    s16 field_1658, field_165A;
    s32 progress;
    CVECTOR inner, outer;
    s32 field_1668, width, angle, field_1674, field_1678, phase;
    u8 unknown_1680[8];
    s32 tint;
    s16 slot, command;
} Variant444EntryState;

extern u8 D_8013D91C[];
void func_8013BDB4(u8 *context);
void func_8013CDC4(u8 *context);
void func_8013D40C(u8 *context);

#endif
