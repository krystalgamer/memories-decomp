#ifndef MEMORIES_MODEL_VARIANT409_ENTRY_H
#define MEMORIES_MODEL_VARIANT409_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "model_variant.h"

/* One 0x34-byte entry of the command table that follows the GsIMAGE table. */
typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, field_1C, travel_start, travel_end, fade_start, fade_end;
    u32 spiral_start;
} EntryConfig409;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag, target;
} EntryProjection409;

/* A nine-row, seventeen-column dome mesh with one colour per row. */
typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
} EntryMesh409;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR colors[2];
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x88];
    VECTOR position;
    u8 unknown_2D8[0x88];
} EntryRibbon409;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x1C];
} EntrySpiral409;

typedef struct {
    u8 unknown_000[0x264];
    u8 color[17][4];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} EntryStreamer409;

typedef struct {
    SVECTOR inner[17], outer[17];
    s32 size, field_114;
} EntryFunnel409;

typedef struct {
    EntryMesh409 mesh;
    EntryRibbon409 ribbons[1];
    EntrySpiral409 spirals[12];
    ModelVariantSheet sheets[2];
    EntryStreamer409 streamers[4];
    EntryFunnel409 funnels[2];
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
    EntryConfig409 *G32 config;
    u32 field_2114;
    GsCOORDUNIT *G32 part;
    s16 field_211C, field_211E;
    s32 width, field_2124, size, field_212C, field_2130, field_2134;
    s32 intensity, field_213C, field_2140, progress, travel;
    s16 field_214C, field_214E, field_2150, field_2152;
    s32 field_2154, field_2158, field_215C, field_2160;
    CVECTOR inner, outer;
    s32 field_216C, field_2170, field_2174, field_2178, field_217C, phase;
    s32 field_2184, field_2188;
    u8 unknown_218C[8];
    s32 ambient;
    s16 slot, command;
} EntryState409;

/* Nine GsIMAGE records (0x1C bytes each) followed by the command table. */
extern u8 gModelVariant409_D37A8[];
void SpanishModelVariant426_DrawMesh(u8 *context);
void SpanishModelVariant426_DrawSpiral(u8 *context);
void FrenchModelVariant426_DrawRibbon(u8 *context);
void SpanishModelVariant426_DrawSheets(u8 *context);
void FrenchModelVariant426_DrawStreamers(u8 *context);

#endif
