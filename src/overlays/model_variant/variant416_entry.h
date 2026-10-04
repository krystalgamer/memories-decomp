#ifndef MEMORIES_MODEL_VARIANT416_ENTRY_H
#define MEMORIES_MODEL_VARIANT416_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, burst_start;
    u8 unknown_24[0xC];
} Variant416EntryConfig;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant416EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    s32 field_70[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_98, field_9A;
    s32 field_9C;
    u8 unknown_A0[8];
} Variant416EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant416EntryFan;

typedef struct {
    Variant416EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant416EntryBand bands[2];
    ModelVariantSheet sheets[3];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant416EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_19D4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant416EntryConfig *G32 config;
    u32 field_1A64;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1A74, field_1A76, field_1A78, field_1A7A;
    s32 field_1A7C, field_1A80, phase;
    u8 unknown_1A88[8];
    s32 tint;
    s16 slot, command;
} Variant416EntryState;

extern u8 D_8013E0D0[];
void func_8013C054(u8 *context);
void func_8013C814(u8 *context);
void func_8013CC68(u8 *context);
void func_8013D1D4(u8 *context);
#endif
