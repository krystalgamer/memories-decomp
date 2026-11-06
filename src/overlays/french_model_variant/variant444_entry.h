#ifndef MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H

#include "../../types.h"
#include "variant337_entry.h"
#include "../spanish_model_variant/variant444_funnels.h"

typedef struct {
    u8 unknown_00[0x40];
    CVECTOR color_40, color_44;
    s32 field_48;
    u8 unknown_4C[0x18];
} Variant444EntryColorRecord;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    ModelVariantTubeView tubes[1];
    ModelVariantSheet sheets[2];
    Funnel411 funnels[2];
    ModelVariantSpokeColorRing spokes[4];
    ModelVariantBeam beams[2];
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
    Variant444EntryConfig *config;
    u32 field_1650;
    GsCOORDUNIT *part;
    s16 field_1658, field_165A;
    s32 progress;
    CVECTOR inner, outer;
    s32 field_1668, width, angle, field_1674, field_1678, phase;
    u8 unknown_1680[8];
    s32 tint;
    s16 slot, command;
} Variant444EntryState;

void FrenchModelVariant175_DrawTube(u8 *context);
void SpanishModelVariant444_DrawSheets(u8 *context);
void SpanishModelVariant444_DrawFunnels(u8 *context);

#endif
