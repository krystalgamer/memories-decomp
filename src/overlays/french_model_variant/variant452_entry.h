#ifndef MEMORIES_FRENCH_MODEL_VARIANT452_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT452_ENTRY_H
#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "variant439_entry.h"
#include "variant452_streamers.h"

/* One 0x38-byte row of header 452's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 scale_end;
    u8 unknown_24[8];
    u32 fade_start, fade_end;
    u8 unknown_34[4];
} Variant452EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWeb webs[3];
    Variant452Streamer streamers[16];
    ModelVariantRay trails[16];
    u8 unknown_25A0[0x360];
    ModelVariantColorBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_32BC[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR position, delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    s16 streamer_rotation, field_336A, field_336C, field_336E;
    s32 field_3370;
    Variant452EntryConfig *config;
    u32 field_3378;
    GsCOORDUNIT *parts[3];
    s16 field_3388, field_338A, field_338C, field_338E;
    s32 field_3390, progress, streamer_radius, field_339C, field_33A0, field_33A4;
    s32 phase, field_33AC;
    u8 unknown_33B0[8];
    s32 tint;
    s16 slot, command;
} Variant452EntryState;

extern GsIMAGE gFrenchModelVariant452_aImages[9];
extern Variant452EntryConfig gFrenchModelVariant452_aConfigs[];
void FrenchModelVariant452_DrawSheets(u8 *context);
void FrenchModelVariant452_DrawStreamers(u8 *context);
void FrenchModelVariant452_DrawLines(u8 *context);
s32 FrenchModelVariant452_HandleRequest(SVECTOR *point, s32 command);
#endif
