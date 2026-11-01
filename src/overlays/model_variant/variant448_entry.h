#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H

#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_effect_state.h"
#include "variant448_fan.h"
#include "variant448_orbit.h"
#include "variant448_radial.h"
#include "variant448_spiral.h"

/* The 0x34-byte effect configuration the entry picks by command. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x14];
} Variant448EntryConfig;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant448EntryQuad;

/* Header 448's work area: one radial record, twelve spiral arms, three webs,
 * three fan units, one orbit, six rings, four spoke rings and one quad, then
 * the packets the helpers draw and the entry's own state. */
typedef struct {
    Variant448Radial radial[1];
    Variant448SpiralArm spiral[12];
    ModelVariantWebNarrow webs[3];
    Variant448FanUnit fans[3];
    Variant448Orbit orbit[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant448EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1A74[0x24];
    MATRIX matrix;
    VECTOR position;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant448EntryConfig *G32 config;
    u32 field_1B14;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1B24, field_1B26, field_1B28, field_1B2A, field_1B2C, field_1B2E, field_1B30, field_1B32;
    s32 field_1B34, field_1B38, phase;
    SVECTOR offset;
    s32 tint;
    s16 slot, command;
} Variant448EntryState;

extern u8 gModelVariant448_D43E4[];
void ModelVariant448_DrawRadialDisc(u8 *context);
void ModelVariant448_DrawFans(u8 *context);
void ModelVariant448_DrawOrbitSheets(u8 *context);

#endif
