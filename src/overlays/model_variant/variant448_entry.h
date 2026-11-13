#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_ENTRY_H

#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_camera_effects.h"
#include "variant448_fan.h"
#include "variant448_orbit.h"
#include "variant448_radial.h"
#include "variant448_spiral.h"

/* The 0x34-byte effect configuration the entry picks by command. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    s32 field_20;
    s32 field_24;
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
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1A74[0x24];
    MATRIX matrix;
    VECTOR position;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant448EntryConfig *config;
    u32 field_1B14;
    GsCOORDUNIT *parts[3];
    s16 field_1B24, field_1B26, field_1B28, field_1B2A, field_1B2C, field_1B2E, field_1B30, field_1B32;
    s32 field_1B34, field_1B38, phase;
    SVECTOR offset;
    s32 tint;
    s16 slot, command;
} Variant448EntryState;

extern GsIMAGE gModelVariant448_aImages[9];
extern Variant448EntryConfig gModelVariant448_aConfigs[];
void ModelVariant448_DrawRadialDisc(u8 *context);
void ModelVariant448_DrawFans(u8 *context);
void ModelVariant448_DrawOrbitSheets(u8 *context);

#endif
