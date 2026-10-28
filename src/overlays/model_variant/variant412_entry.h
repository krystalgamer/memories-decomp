#ifndef MEMORIES_MODEL_VARIANT412_ENTRY_H
#define MEMORIES_MODEL_VARIANT412_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
    u32 sweep_start, sweep_end;
    s32 height;
} Variant412EntryConfig;

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
} Variant412EntryTrail;

/* Six sprites at work + 0x2FC, released from the opposite slot. */
typedef struct {
    SVECTOR v0[6], v1[6], v2[6], v3[6], origin[6];
    CVECTOR color;
    u8 unknown_F4[0x10];
    s32 scale[6];
    s32 field_11C[6];
    s32 field_134[6];
    s32 done[6];
} Variant412EntryFlares;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant412EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant412EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant412EntryQuad;

typedef struct {
    Variant412EntryTrail trails[1];
    Variant412EntryFlares flares;
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
    u8 unknown_1D14[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[6];
    VECTOR offsets[6];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant412EntryConfig *G32 config;
    u32 field_1E64;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1E74, field_1E76;
    s32 sweep;
    s16 field_1E7C, field_1E7E;
    s32 field_1E80, field_1E84, phase;
    u8 unknown_1E8C[8];
    s32 tint;
    s16 slot, command;
} Variant412EntryState;

extern u8 D_8013CD88[];
void func_8013C340(u8 *context);
void func_8013C8B8(u8 *context);
#endif
