#ifndef MEMORIES_MODEL_VARIANT423_ENTRY_H
#define MEMORIES_MODEL_VARIANT423_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x10];
} Variant423EntryConfig;

typedef struct {
    u8 unknown_00[0xB4];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0;
    u8 unknown_F4[0x14];
} Variant423EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant423EntryFan;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant423EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant423EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E74[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant423EntryConfig *G32 config;
    u32 field_F04;
    GsCOORDUNIT *G32 parts[3];
    s16 field_F14, field_F16, field_F18, field_F1A, field_F1C, field_F1E;
    s32 field_F20;
    s32 phase;
    u8 unknown_F28[8];
    s32 tint;
    s16 slot, command;
} Variant423EntryState;

extern u8 D_8013DB38[];
void func_8013BF18(u8 *context);
void func_8013C6E4(u8 *context);
void func_8013CBDC(u8 *context);
#endif
