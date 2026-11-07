#ifndef SPANISH_MODEL_VARIANT411_FUNNELS_H
#define SPANISH_MODEL_VARIANT411_FUNNELS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} Funnel411;

typedef struct {
    u8 unknown_000[0x54C];
    ModelVariantSheet sheet;
    u8 unknown_5E4[0x98];
    Funnel411 funnels[2];
    u8 unknown_8AC[0xC34];
    POLY_GT4 quad;
    u8 unknown_1514[0xE4];
    s32 origin[3];
    u8 unknown_1604[8];
    s32 direction[3];
    u8 unknown_1618[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1640[4];
    s32 step;
    u8 unknown_1648[4];
    ModelVariantGrowFadeTiming *timing;
    u8 unknown_1650[0x10];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 spin;
    u8 unknown_166C[0x10];
    s32 phase;
} Funnel411State;

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

#endif
