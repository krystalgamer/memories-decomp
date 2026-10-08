#ifndef MEMORIES_MODEL_VARIANT401_ENTRY_H
#define MEMORIES_MODEL_VARIANT401_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
} Variant401EntryConfig;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant401EntryRecord;

typedef struct {
    u8 unknown_00[0x1D4];
    s32 offset[9];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_250, field_252;
    s32 field_254;
    u8 unknown_258[0x24];
} Variant401EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant401EntryFan;

typedef struct {
    Variant401EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant401EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant401EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_flat_end[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant401EntryConfig *G32 config;
    u32 field_1A64;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1A74, field_1A76, field_1A78, field_1A7A;
    s32 field_1A7C, field_1A80, phase;
    u8 unknown_1A88[8];
    s32 tint;
    s16 slot, command;
} Variant401EntryState;

extern u8 D_8013E124[];
void func_8013BFE4(u8 *context);
void func_8013C764(u8 *context);
void func_8013CCA4(u8 *context);
void func_8013D210(u8 *context);
#endif
