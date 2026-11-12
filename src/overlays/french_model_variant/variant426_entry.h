#ifndef MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H

#include "../../types.h"
#include "../model_variant/variant458_streamers.h"
#include "variant425_entry.h"

typedef struct {
    ModelVariantMesh mesh;
    EntryRibbon425 ribbons[1];
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[2];
    Variant458Streamer streamers[4];
    Funnel423 funnels[2];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1590[0x34];
    POLY_GT4 special;
    u8 unknown_15F8[0x34];
    ModelVariantPackets packets;
    POLY_FT4 flat[2];
    POLY_FT4 flat2[2];
    u8 unknown_1724[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    ModelVariantSpiralConfig *config;
    u32 field_17A0;
    GsCOORDUNIT *part;
    s16 field_17A8, field_17AA;
    s32 width, field_17B0, size, field_17B8, field_17BC, field_17C0;
    s32 intensity, field_17C8, field_17CC, progress, travel;
    s16 spiral_angle, field_17DA, spiral_size, field_17DE;
    ModelVariantStreamerParams streamer;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, field_1800, field_1804, field_1808, phase;
    s32 field_1810, field_1814;
    u8 unknown_1818[8];
    s32 ambient;
    s16 slot, command;
} EntryState426;

extern GsIMAGE gFrenchModelVariant426_aImages[9];
extern ModelVariantSpiralConfig gFrenchModelVariant426_aConfigs[];

void SpanishModelVariant426_DrawMesh(u8 *context);
void SpanishModelVariant426_DrawSpiral(u8 *context);
void FrenchModelVariant426_DrawRibbon(u8 *context);
void SpanishModelVariant426_DrawSheets(u8 *context);
void FrenchModelVariant426_DrawStreamers(u8 *context);

#endif
