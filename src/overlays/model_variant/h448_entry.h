#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H

#include "../../types.h"
#include "../model_variant/h320_entry_pal.h"
#include "../../game/model_camera_effects.h"
#include "h448_fan.h"
#include "h448_orbit.h"
#include "h448_radial.h"
#include "h448_spiral.h"

/* The 0x34-byte effect configuration the entry picks by command: the radial,
 * fan and quad sizes grow from start to grow_end, and from shrink_start the
 * radial shrinks and the fan arms settle. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    s32 grow_end;
    u32 shrink_start;
    u8 unknown_28[0xC];
} Variant448EntryConfig;

/* Header 448's work area: one radial record, twelve spiral arms, three webs,
 * three fan units, one orbit, six rings, four spoke rings and one quad, then
 * the packets the helpers draw and the entry's own state. */
typedef struct {
    Variant448Radial radial[1];
    ModelVariantShadedRay spiral[12];
    ModelVariantWebNarrow webs[3];
    Variant448FanUnit fans[3];
    Variant448Orbit orbit[1];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1A74[0x10];
    GsGLINE line;
    MATRIX matrix;
    VECTOR position;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant448EntryConfig *config;
    u32 field_1B14;
    GsCOORDUNIT *parts[3];
    s16 spiral_angle, field_1B26, spiral_size, field_1B2A, field_1B2C, field_1B2E, scale, field_1B32;
    s32 field_1B34, quad_angle, phase;
    SVECTOR offset;
    s32 tint;
    s16 slot, command;
} Variant448EntryState;

extern GsIMAGE gModelVariant448_aImages[9];
extern Variant448EntryConfig gModelVariant448_aConfigs[];
void ModelVariant448_DrawRadialDisc(Variant448EntryState *context);
void ModelVariant448_DrawFans(Variant448EntryState *context);
void ModelVariant448_DrawOrbitSheets(Variant448EntryState *context);

#endif
