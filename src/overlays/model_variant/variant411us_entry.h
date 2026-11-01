#ifndef MEMORIES_MODEL_VARIANT411US_ENTRY_H
#define MEMORIES_MODEL_VARIANT411US_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* The 0x38-byte config: the header-412 config without its height word. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
    u32 sweep_start, sweep_end;
} Variant411UsEntryConfig;

/* Ten sprites at the start of the work area. */
typedef struct {
    SVECTOR v0[10], v1[10], v2[10], v3[10], origin[10];
    CVECTOR color;
    s32 scale[10];
    s32 field_1BC[10];
    s32 field_1E4[10];
    s32 field_20C[10];
    s32 done[10];
    u8 unknown_25C[0xA0];
} Variant411UsEntryTrail;

/* The 32-sprite swarm at work + 0x2FC, released towards the opposite slot. */
typedef struct {
    SVECTOR v0[32], v1[32], v2[32], v3[32], origin[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 scale[32];
    s32 field_594[32];
    s32 field_614[32];
    s32 done[32];
} Variant411UsEntrySwarm;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant411UsEntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant411UsEntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant411UsEntryQuad;

/* ModelVariantSheet with its three trailing words named. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} Variant411UsEntrySheet;

/* ModelVariantWebNarrow with the words it clears named. */
typedef struct {
    SVECTOR near[4][6], far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale, done, field_19C;
} Variant411UsEntryWeb;

typedef struct {
    Variant411UsEntryTrail trails[1];
    Variant411UsEntrySwarm swarm;
    Variant411UsEntryRecord records[16];
    Variant411UsEntryWeb webs[3];
    Variant411UsEntryBand bands[1];
    Variant411UsEntrySheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant411UsEntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_22C4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[32];
    VECTOR offsets[32];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant411UsEntryConfig *G32 config;
    u32 field_2754;
    GsCOORDUNIT *G32 parts[3];
    s16 field_2764, field_2766;
    s32 sweep;
    s16 field_276C, field_276E;
    s32 field_2770, field_2774, phase;
    u8 unknown_277C[8];
    s32 tint;
    s16 slot, command;
} Variant411UsEntryState;

extern u8 gModelVariant411_D1D88[];
void FrenchModelVariant428_DrawSwarm(u8 *context);
void FrenchModelVariant428_DrawQuads(u8 *context);
#endif
