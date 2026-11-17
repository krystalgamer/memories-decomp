#ifndef MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant425_entry_pal.h"
#include "variant426_ribbon.h"

typedef struct {
    ModelVariantMesh mesh;
    EntryRibbon425 ribbons[1];
    ModelVariantShadedRay spirals[12];
    ModelVariantSheet sheets[2];
    ModelVariantStreamer streamers[4];
    ModelVariantFunnel funnels[2];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1EB4[0x34];
    POLY_GT4 special;
    u8 unknown_1F1C[0x34];
    ModelVariantPackets packets;
    POLY_FT4 flat[2];
    POLY_FT4 flat2[2];
    u8 unknown_2098[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantSpiralConfig *config;
    u32 field_2114;
    GsCOORDUNIT *part;
    s16 field_211C, field_211E;
    s32 width, field_2124, size, field_212C, field_2130, field_2134;
    s32 intensity, field_213C, field_2140, progress, travel;
    s16 spiral_angle, field_214E, spiral_size, field_2152;
    ModelVariantStreamerParams streamer;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, field_2174, field_2178, field_217C, phase;
    s32 field_2184, field_2188;
    u8 unknown_218C[8];
    s32 ambient;
    s16 slot, command;
} EntryState426;

extern GsIMAGE gFrenchModelVariant426_aImages[9];
extern ModelVariantSpiralConfig gFrenchModelVariant426_aConfigs[];

void SpanishModelVariant426_DrawMesh(EntryState426 *context);
void SpanishModelVariant426_DrawSpiral(EntryState426 *context);
void FrenchModelVariant426_DrawRibbon(Ribbon426Work *context);
void SpanishModelVariant426_DrawSheets(EntryState426 *context);
void FrenchModelVariant426_DrawStreamers(EntryState426 *context);

#endif
