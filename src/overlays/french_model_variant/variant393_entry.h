#ifndef MEMORIES_FRENCH_MODEL_VARIANT393_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT393_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR inner, ribbon, outer;
    u8 parts[3], unknown_0F;
    u16 vertices[3];
    u8 unknown_16[6];
    u16 count, unknown_1E;
    s32 mode;
    u32 start, end, field_2C, fade_start, fade_end;
    s32 offset[3];
} Variant393EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant337EntryRecord records[1];
    ModelVariantPointSheet rings[2];
    Variant337EntryStreamer streamers[2];
    ModelVariantPackets packets;
    POLY_FT4 flat_textured[2], extra_flat[2];
    u8 unknown_D0C[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR positions[3], direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant393EntryConfig *config;
    u32 unknown_DB8;
    GsCOORDUNIT *parts[3];
    s32 part_index;
    s16 ribbon_length, field_DCE, field_DD0, width;
    s32 field_DD4, streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    s32 ribbon_wave, field_DEC, field_DF0, field_DF4;
    ModelVariantEntryTail tail;
} Variant393EntryState;

extern GsIMAGE gFrenchModelVariant393_aImages[9];
extern Variant393EntryConfig gFrenchModelVariant393_aConfigs[];
void ModelVariant376_DrawRibbon(u8 *context);
void ModelVariant376_DrawRings(u8 *context);
void FrenchModelVariant393_DrawStreamers(u8 *context);
typedef char Variant393EntryConfigSize[(sizeof(Variant393EntryConfig) == 0x44) ? 1 : -1];
typedef char Variant393EntryStateSize[(sizeof(Variant393EntryState) == 0xE0C) ? 1 : -1];
#endif
