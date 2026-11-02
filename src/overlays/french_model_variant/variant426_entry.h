#ifndef MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT426_ENTRY_H

#include "../../types.h"
#include "../model_variant/variant458_streamers.h"
#include "variant425_entry.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, field_1C, travel_start, travel_end, fade_start, fade_end;
    u32 field_30;
} EntryConfig426;

typedef struct {
    Mesh425Rows mesh;
    EntryRibbon425 ribbons[1];
    EntrySpiral425 spirals[12];
    ModelVariantSheet sheets[2];
    Variant458Streamer streamers[4];
    Funnel423 funnels[2];
    POLY_GT4 extra, funnel_packet;
    u8 unknown_1590[0x34];
    POLY_GT4 special;
    u8 unknown_15F8[0x34];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    POLY_FT4 flat2[2];
    u8 unknown_1724[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig426 *G32 config;
    u32 field_17A0;
    GsCOORDUNIT *G32 part;
    s16 field_17A8, field_17AA;
    s32 width, field_17B0, size, field_17B8, field_17BC, field_17C0;
    s32 intensity, field_17C8, field_17CC, progress, travel;
    s16 field_17D8, field_17DA, field_17DC, field_17DE;
    s32 field_17E0, field_17E4, field_17E8, field_17EC;
    CVECTOR inner, outer;
    s32 field_17F8, field_17FC, field_1800, field_1804, field_1808, phase;
    s32 field_1810, field_1814;
    u8 unknown_1818[8];
    s32 ambient;
    s16 slot, command;
} EntryState426;

extern u8 gFrenchModelVariant426_D3874[];

void SpanishModelVariant426_DrawMesh(u8 *context);
void SpanishModelVariant426_DrawSpiral(u8 *context);
void FrenchModelVariant426_DrawRibbon(u8 *context);
void SpanishModelVariant426_DrawSheets(u8 *context);
void FrenchModelVariant426_DrawStreamers(u8 *context);

#endif
