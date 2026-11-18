#ifndef FRENCH_MODEL_VARIANT473_ENTRY_H
#define FRENCH_MODEL_VARIANT473_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/model_variant.h"
#include "e593_sheets_pal.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start;
    u32 grow_end;
    u8 unknown_20[4];
    u32 fade_begin, fade_end;
} EntryConfig473;

/* One 0x22C-byte ribbon: a nine-point spine and a copy of it moved along the
 * view, both projected, the per-point screen angle and projected width, the
 * inner and outer colour rows and scale the entry seeds, the growth progress
 * (staggered below zero), fade and state, the position, and per point the
 * depth and the width's screen offset. */
typedef struct {
    SVECTOR a[9];
    long sa[9];
    s32 angle[9];
    SVECTOR b[9];
    long sb[9];
    s32 width[9];
    CVECTOR inner[9], outer[9];
    VECTOR scale;
    s16 field_178, field_17A;
    s32 field_17C, progress, fade, state;
    u8 unknown_18C[0x48];
    VECTOR position;
    s32 otz[9];
    s16 ox[9];
    s16 oy[9];
} EntryRibbon473;

typedef struct {
    ModelVariantGridSheet meshes[5];
    EntryRibbon473 ribbons[5];
    ModelVariantShadedRay spirals[12];
    ModelVariantSheet sheets[6];
    ModelVariantStreamer streamers[2];
    ModelVariantFunnel funnels[5];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_395C[0x34];
    ModelVariantPackets packets;
    /* The entry fills two POLY_FT4 templates here; the 458-style streamer
     * drawer this image keeps reads the same bytes as one POLY_G4. */
    union {
        POLY_FT4 ft4[2];
        POLY_G4 g4;
    } flat;
    POLY_FT4 additional[2];
    u8 unknown_3AD8[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR origins[5], direction, ribbon_directions[5];
    DVECTOR screen_delta;
    VECTOR view_delta;
    ModelVariantClock clock;
    EntryConfig473 *config;
    u32 field_3BF4;
    GsCOORDUNIT *part;
    s16 field_3BFC, field_3BFE;
    s32 width, field_3C04, size, field_3C0C, field_3C10, field_3C14;
    s32 intensity, field_3C1C, field_3C20, progress;
    ModelVariantSpiralParams spiral;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    CVECTOR inner, outer;
    s32 spin, ribbon_sway, ribbon_wave, field_3C54, field_3C58, phase;
    s32 field_3C60, field_3C64;
    u8 unknown_3C68[8];
    s32 ambient;
    s16 slot, command;
} EntryState473;

extern GsIMAGE gFrenchModelVariant473_aImages[9];
extern EntryConfig473 gFrenchModelVariant473_aConfigs[];
void FrenchModelVariant385_DrawGrid(EntryState473 *context);
void SpanishModelVariant473_DrawSpiral(EntryState473 *context);
void FrenchModelVariant473_DrawRibs(EntryState473 *context);
void SpanishModelVariant473_DrawSheets(Sheets473State *context);
void SpanishModelVariant473_DrawFunnels(EntryState473 *context);

#endif
