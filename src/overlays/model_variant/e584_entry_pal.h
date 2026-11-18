#ifndef MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT444_ENTRY_H

#include "../../types.h"
#include "e503_h320_entry_pal.h"
#include "e577_beams_pal.h"

/* One command record of the French header-444 entry's table. The funnels read
   the first record's fade start (old blob + 0x114) whatever the command. */
typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 sheet_start, tube_start, tube_end, fade_start, fade_end, end;
} Variant444EntryConfig;

extern GsIMAGE gFrenchModelVariant444_aImages[9];
extern Variant444EntryConfig gFrenchModelVariant444_aConfigs[];

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    ModelVariantTube tubes[1];
    ModelVariantSheet sheets[2];
    ModelVariantFunnel funnels[2];
    ModelVariantSpokeColorRing spokes[4];
    ModelVariantBeam extras[2];
    Beam411 beams[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], plain, paged, extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1598[0x4C];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant444EntryConfig *config;
    u32 field_1650;
    GsCOORDUNIT *part;
    s16 field_1658, field_165A;
    s32 progress;
    CVECTOR inner, outer;
    s32 spin, width, angle, beam_spin, field_1678;
    ModelVariantEntryTail tail;
} Variant444EntryState;

void FrenchModelVariant175_DrawTube(Variant444EntryState *context);
void SpanishModelVariant444_DrawSheets(Variant444EntryState *context);
void SpanishModelVariant444_DrawFunnels(Variant444EntryState *context);

#endif
