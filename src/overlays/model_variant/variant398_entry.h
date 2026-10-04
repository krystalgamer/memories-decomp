#ifndef MEMORIES_MODEL_VARIANT398_ENTRY_H
#define MEMORIES_MODEL_VARIANT398_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant439_entry.h"

/* Header 398's framebuffer ring: three rows of seventeen points and a colour
 * per point, cycling through eight hues. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR color[17];
    s32 scale, count;
} Variant398EntryScreenRing;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant439EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant439EntryFan fans[1];
    Variant398EntryScreenRing screen_rings[3];
    ModelVariantCurtain curtains[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, curtain_poly;
    POLY_FT4 flat_textured[2];
    u8 unknown_18C0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant439EntryConfig *G32 config;
    u32 field_1950;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1960, field_1962, field_1964, field_1966;
    s32 field_1968, field_196C;
    CVECTOR outer, inner;
    s32 field_1978, phase;
    u8 unknown_1980[8];
    s32 tint;
    s16 slot, command;
} Variant398EntryState;

extern u8 D_8013DFE0[];
void func_8013C2B8(u8 *context);
void func_8013C8AC(u8 *context);
void func_8013D02C(u8 *context);
void func_8013DA44(u8 *context);
#endif
