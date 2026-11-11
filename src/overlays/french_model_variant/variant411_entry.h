#ifndef FRENCH_MODEL_VARIANT411_ENTRY_H
#define FRENCH_MODEL_VARIANT411_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "variant411_tube.h"
#include "../spanish_model_variant/variant411_funnels.h"
#include "../model_variant/variant411_beams.h"
#include "../../game/model_slot_getters.h"

typedef struct {
    ModelVariantTubeView tubes[1];
    ModelVariantSheet sheets[2];
    ModelVariantFunnel funnels[2];
    ModelVariantSpokeColorRing rings[4];
    ModelVariantBeam extras[2];
    Beam411 beams[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra, ring_packet, secondary;
    POLY_FT4 flat[2];
    u8 unknown_1598[0x4C];
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count, elapsed, animation, step;
    s32 fade;
    EntryConfig411 *config;
    u32 field_1650;
    GsCOORDUNIT *part;
    s32 progress;
    s16 field_165C, field_165E;
    CVECTOR inner, outer;
    s32 spin, width, angle, field_1674, field_1678, phase;
    u8 unknown_1680[8];
    s32 ambient;
    s16 slot, command;
} EntryState411;

void FrenchModelVariant411_DrawTube(u8 *context);
void SpanishModelVariant411_DrawFadingSheets(u8 *context);
void SpanishModelVariant411_DrawFunnels(u8 *context);

#endif
