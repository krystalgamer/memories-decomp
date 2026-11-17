#ifndef MEMORIES_FRENCH_MODEL_VARIANT460_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT460_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR streamer, outer, ribbon, sheet;
    u8 parts[8], unknown_18[8];
    s32 count, mode, kind;
    u32 sheet_start, ribbon_start, unknown_34, phase3, phase4;
} Variant460EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_238, field_23C, count, field_244, state, length;
    VECTOR velocity;
    u8 unknown_260[0x2E8 - 0x260];
} Variant460EntryRecord;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant460EntryRecord records[8];
    ModelVariantSheetSet sheets[16];
    ModelVariantStrand strands[6];
    ModelVariantFlaggedStreamer streamers[2];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    GsLINE line;
    MATRIX matrix;
    SVECTOR target;
    MATRIX matrices[8];
    VECTOR targets[8];
    VECTOR directions[8];
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant460EntryConfig *config;
    u32 field_2E80;
    GsCOORDUNIT *parts[8];
    s16 field_2EA4, strand_start, strand_end, width;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    s32 ribbon_wave, ribbon_phase;
    ModelVariantEntryTail tail;
} Variant460EntryState;

extern GsIMAGE gFrenchModelVariant460_aImages[9];
extern Variant460EntryConfig gFrenchModelVariant460_aConfigs[];
void ModelVariant443_DrawRibbons(Variant460EntryState *context);
void ModelVariant443_DrawSheets(Variant460EntryState *entry);
s32 FrenchModelVariant460_HandleRequest(SVECTOR *point, s32 command);
#endif
