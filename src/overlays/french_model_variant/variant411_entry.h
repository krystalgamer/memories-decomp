#ifndef FRENCH_MODEL_VARIANT411_ENTRY_H
#define FRENCH_MODEL_VARIANT411_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "variant411_tube.h"
#include "../spanish_model_variant/variant411_funnels.h"
#include "../model_variant/variant411_beams.h"
#include "../../game/model_get_frame_step.h"

typedef struct {
    u8 unknown_00[4], part, unknown_05[7];
    u32 sheet_start, expansion_start, expansion_end, fade_start, fade_end;
    u32 field_20;
    s32 offset[3];
} EntryConfig411;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR colors[8], inner_color, outer_color;
    s32 size, field_AC;
} EntryRing411;

typedef struct {
    u8 unknown_00[0xD8];
    CVECTOR inner, outer;
    VECTOR scale;
    u8 unknown_F0[0x3C];
} EntryExtra411;

typedef struct {
    Tube411 tubes[1];
    ModelVariantSheet sheets[2];
    Funnel411 funnels[2];
    EntryRing411 rings[4];
    EntryExtra411 extras[2];
    Beam411 beams[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra, ring_packet, secondary;
    POLY_FT4 flat[2];
    u8 unknown_1598[0x4C];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation, step;
    s32 fade;
    EntryConfig411 *G32 config;
    u32 field_1650;
    GsCOORDUNIT *G32 part;
    s32 progress;
    s16 field_165C, field_165E;
    CVECTOR inner, outer;
    s32 spin, width, angle, field_1674, field_1678, phase;
    u8 unknown_1680[8];
    s32 ambient;
    s16 slot, command;
} EntryState411;

void func_8013BE60(u8 *context);
void func_8013CE58(u8 *context);
void func_8013D4A4(u8 *context);

#endif
