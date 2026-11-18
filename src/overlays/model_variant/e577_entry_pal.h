#ifndef FRENCH_MODEL_VARIANT411_ENTRY_H
#define FRENCH_MODEL_VARIANT411_ENTRY_H

#include "../../types.h"
#include "e568_entry_pal.h"
#include "../model_variant/e577_beams_pal.h"
#include "../../game/model_slot_getters.h"

/* One command record of the header-411 entry's table. The funnels read
   the first record's fade start (old blob + 0x114) whatever the command. */
typedef struct {
    u8 unknown_00[4], part, unknown_05[7];
    u32 sheet_start, expansion_start, expansion_end, fade_start, fade_end;
    u32 field_20;
    s32 offset[3];
} EntryConfig411;

extern GsIMAGE gSpanishModelVariant411_aImages[9];
extern EntryConfig411 gSpanishModelVariant411_aConfigs[];

typedef struct {
    ModelVariantTube tubes[1];
    ModelVariantSheet sheets[2];
    ModelVariantFunnel funnels[2];
    ModelVariantSpokeColorRing rings[4];
    ModelVariantBeam extras[2];
    Beam411 beams[16];
    ModelVariantPackets packets;
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
    s32 spin, width, angle, beam_spin, field_1678, phase;
    u8 unknown_1680[8];
    s32 ambient;
    s16 slot, command;
} EntryState411;

void FrenchModelVariant411_DrawTube(EntryState411 *context);
void SpanishModelVariant411_DrawFadingSheets(EntryState411 *context);
void SpanishModelVariant411_DrawFunnels(EntryState411 *context);

#endif
