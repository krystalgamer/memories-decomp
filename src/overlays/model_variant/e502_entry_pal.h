#ifndef MEMORIES_FRENCH_MODEL_VARIANT336_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT336_ENTRY_H
#include "../../types.h"
#include "h320_entry_pal.h"

typedef struct {
    u8 unknown_00[4];
    u8 part0, part1, unknown_06[10];
    u32 start, end;
    u8 unknown_18[12];
} Variant336EntryConfig;

/* One 0x3C0-byte ribbon: the projected spine, the style the entry seeds
 * (the ribbon drawer shades by style.inner), the ribbon's origin and end,
 * its state, count and extent, and the projection flags and offsets. */
typedef struct {
    ModelVariantStripSpine spine;
    ModelVariantStripStyle style;
    u8 unknown_240[0x88];
    VECTOR origin, end;
    s32 state, count, extent;
    long flag[17];
    ModelVariantStripOffsets offsets;
} Variant336EntryRecord;

/* Only the entry's measured view, not an allocation-capacity claim. */
typedef struct {
    Variant336EntryRecord records[4];
    ModelVariantPointSheet sheets[8];
    ModelVariantFlaggedStreamer streamers[2];
    ModelVariantStrandWide strands[6];
    ModelVariantPackets packets;
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    GsLINE line;
    u8 unknown_1F20[0x1F34 - 0x1F20];
    s32 origin[3];
    MATRIX matrix0, matrix1;
    /* The ribbon helper's span from a ribbon's origin to its end. */
    VECTOR span;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant336EntryConfig *config;
    u8 unknown_1FDC[4];
    GsCOORDUNIT *part0, *part1;
    s32 field_1FE8, sheet_count;
    s16 segment_start, segment_end;
    s32 wave0, wave1, extent, phase;
    u8 unknown_2004[8];
    s32 brightness;
    s16 slot, command;
    s32 rotation[3];
} Variant336EntryState;

extern GsIMAGE gFrenchModelVariant336_aImages[9];
extern Variant336EntryConfig gFrenchModelVariant336_aConfigs[];
s32 FrenchModelVariant336_HandleRequest(Variant336EntryState *context, s32 command);
void FrenchModelVariant336_DrawRibbon(Variant336EntryState *context);
void FrenchModelVariant336_DrawSheets(Variant336EntryState *context);
void FrenchModelVariant336_DrawStreamers(Variant336EntryState *context);
#endif
