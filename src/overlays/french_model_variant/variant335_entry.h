#ifndef FRENCH335_ENTRY_VIEW_H
#define FRENCH335_ENTRY_VIEW_H
#include "../../types.h"
#include "variant337_entry.h"
#include "variant335_rings.h"

typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 start;
    u32 field_10, field_14, field_18, field_1C, field_20, field_24, field_28;
    u32 end;
} Model335EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    ModelVariantStripStyle style;
    u8 unknown_240[0x394 - 0x240];
} Model335EntryRecord;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Model335EntryRecord records[3];
    ModelVariantPointSheet sheets[4];
    Variant335Ring rings[3];
    ModelVariantStreamer short_streamers[6];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[3];
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    u8 unknown_2DB8[16];
    MATRIX matrix;
    VECTOR origin, end, delta;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Model335EntryConfig *config;
    u8 unknown_2E64[4];
    GsCOORDUNIT *part;
    s32 travel_angle, lift, spin, orbit_diameter;
    s32 rotation[3], extent;
    s16 count, field_2E8E, field_2E90, width, field_2E94, field_2E96;
    s32 wave0, wave1, phase;
    u8 unknown_2EA4[8];
    s32 brightness;
    s16 slot, command;
} Model335EntryState;

s32 FrenchModelVariant335_HandleRequest(SVECTOR *point, s32 command);
extern u8 gFrenchModelVariant335_D28EC[];
void FrenchModelVariant335_DrawRibbons(u8 *context);
void FrenchModelVariant335_DrawSheets(u8 *context);
void FrenchModelVariant335_DrawRings(u8 *context);
void FrenchModelVariant335_DrawStreamers(u8 *context);
#endif
