#ifndef MEMORIES_MODEL_VARIANT413_ENTRY_H
#define MEMORIES_MODEL_VARIANT413_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant412_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0xC];
    u32 origin_end;
    u32 sweep_start, sweep_end;
} Variant413EntryConfig;

/* Three groups of 32 sprites at the start of the work area, drawn by
 * func_8013C2F0 (image132118_swarm.c). */
typedef struct {
    SVECTOR v0[32], v1[32], v2[32], v3[32], origin[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 scale[32];
    s32 field_594[32];
    s32 field_614[32];
    s32 done[32];
} Variant413EntryGroup;

typedef struct {
    Variant413EntryGroup groups[3];
    Variant412EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant412EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant412EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_2DF0[0x24];
    MATRIX matrix;
    MATRIX origins[3];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[3][32];
    VECTOR offsets[3][32];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant413EntryConfig *G32 config;
    u32 field_3AE0;
    GsCOORDUNIT *G32 parts[3];
    s16 field_3AF0, field_3AF2;
    s32 sweep;
    s16 field_3AF8, field_3AFA;
    s32 field_3AFC, field_3B00, phase;
    u8 unknown_3B08[8];
    s32 tint;
    s16 slot, command;
} Variant413EntryState;

extern u8 D_8013C8C0[];
void func_8013C2F0(u8 *context);
#endif
