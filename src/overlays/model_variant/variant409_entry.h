#ifndef MEMORIES_MODEL_VARIANT409_ENTRY_H
#define MEMORIES_MODEL_VARIANT409_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "model_variant.h"

typedef struct {
    u8 unknown_000[0x264];
    u8 color[17][4];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} EntryStreamer409;

typedef struct {
    ModelVariantMesh mesh;
    ModelVariantLongRibbon ribbons[1];
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[2];
    EntryStreamer409 streamers[4];
    ModelVariantCurtain funnels[2];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1EB4[0x34];
    POLY_GT4 special;
    u8 unknown_1F1C[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    POLY_FT4 band[2];
    u8 unknown_2098[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
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
} EntryState409;

/* Nine GsIMAGE records (0x1C bytes each) followed by the command table. */
extern GsIMAGE gModelVariant409_aImages[9];
extern ModelVariantSpiralConfig gModelVariant409_aConfigs[];
void SpanishModelVariant426_DrawMesh(u8 *context);
void SpanishModelVariant426_DrawSpiral(u8 *context);
void FrenchModelVariant426_DrawRibbon(u8 *context);
void SpanishModelVariant426_DrawSheets(u8 *context);
void FrenchModelVariant426_DrawStreamers(u8 *context);

#endif
