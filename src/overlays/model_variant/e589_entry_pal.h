#ifndef MEMORIES_FRENCH_MODEL_VARIANT423_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT423_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/model_variant.h"
#include "e589_ribbon_pal.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, grow_end, field_20, fade_start, fade_end;
} EntryConfig423;

typedef struct {
    ModelVariantMesh mesh;
    Ribbon423 ribbons[1];
    ModelVariantShadedRay spirals[12];
    ModelVariantSheet sheets[2];
    ModelVariantStreamer streamers[2];
    ModelVariantFunnel funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1B94[0x34];
    POLY_GT4 special;
    u8 unknown_1BFC[0x34];
    ModelVariantPackets packets;
    /* The entry fills two POLY_FT4 templates here; the 458-style streamer
     * drawer this image keeps reads the same bytes as one POLY_G4. */
    union {
        POLY_FT4 ft4[2];
        POLY_G4 g4;
    } flat;
    u8 unknown_1D28[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    EntryConfig423 *config;
    u32 field_1DA4;
    GsCOORDUNIT *part;
    s16 field_1DAC, field_1DAE;
    s32 width, field_1DB4, size, field_1DBC, field_1DC0, field_1DC4;
    s32 intensity, field_1DCC, field_1DD0, progress;
    ModelVariantSpiralParams spiral;
    ModelVariantStreamerParams streamer;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, ribbon_wave, field_1E04, field_1E08, phase;
    s32 field_1E10, field_1E14;
    u8 unknown_1E18[8];
    s32 ambient;
    s16 slot, command;
} EntryState423;

extern GsIMAGE gFrenchModelVariant423_aImages[9];
extern EntryConfig423 gFrenchModelVariant423_aConfigs[];
void SpanishModelVariant423_DrawMesh(EntryState423 *context);
void SpanishModelVariant423_DrawSpiral(EntryState423 *context);
void FrenchModelVariant423_DrawRibbon(EntryState423 *context);
void SpanishModelVariant423_DrawSheets(EntryState423 *context);
void FrenchModelVariant423_DrawFunnel(EntryState423 *context);

#endif
