#ifndef MEMORIES_MODEL_VARIANT454_ENTRY_H
#define MEMORIES_MODEL_VARIANT454_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_effect_state.h"
#include "variant412_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0xC];
    u32 origin_end;
    u32 sweep_start, sweep_end;
} Variant454EntryConfig;

/* One group of 16 sprites at the start of the work area. */
typedef struct {
    SVECTOR v0[16], v1[16], v2[16], v3[16], origin[16];
    CVECTOR color;
    u8 unknown_284[0x10];
    s32 scale[16];
    s32 field_2D4[16];
    s32 field_314[16];
    s32 done[16];
    MATRIX origins[16];
    VECTOR positions[16];
    VECTOR offsets[16];
} Variant454EntryGroup;

/* A cluster of 10 sprites after the group. */
typedef struct {
    SVECTOR v0[10], v1[10], v2[10], v3[10], origin[10];
    CVECTOR color;
    s32 scale[10];
    s32 field_1BC[10];
    s32 field_1E4[10];
    s32 field_20C[10];
    s32 done[10];
    u8 unknown_25C[0xA0];
} Variant454EntryCluster;

typedef struct {
    Variant454EntryGroup groups[1];
    Variant454EntryCluster clusters[1];
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
    POLY_FT4 flat_textured[2], sprite, glow;
    u8 unknown_236C[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant454EntryConfig *G32 config;
    u32 field_23FC;
    GsCOORDUNIT *G32 parts[3];
    s16 field_240C, field_240E;
    s32 sweep;
    s16 field_2414, field_2416;
    s32 field_2418, field_241C;
    SVECTOR orbit;
    s32 tint;
    s32 phase;
    s16 slot, command;
} Variant454EntryState;

extern u8 gModelVariant454_D1EFC[];
void FrenchModelVariant471_DrawQuadGroup(u8 *context);
void FrenchModelVariant471_DrawQuads(u8 *context);
#endif
