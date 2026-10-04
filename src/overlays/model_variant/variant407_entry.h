#ifndef MEMORIES_MODEL_VARIANT407_ENTRY_H
#define MEMORIES_MODEL_VARIANT407_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u16 petal_size, petal_radius;
    u8 unknown_20[4];
    u32 start;
    u8 unknown_28[0xC];
    u32 track_end, sweep_start, sweep_end, sweep_angle;
} Variant407EntryConfig;

typedef struct {
    SVECTOR v0[48], v1[48], v2[48], v3[48], origin[48];
    CVECTOR color;
    u8 unknown_784[0x10];
    s32 scale[48];
    s32 field_854[48];
    s32 field_914[48];
    s32 done[48];
} Variant407EntryPetals;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant407EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant407EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant407EntryQuad;

typedef struct {
    Variant407EntryPetals petals;
    Variant407EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant407EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant407EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_2348[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[48];
    VECTOR offsets[48];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant407EntryConfig *G32 config;
    u32 field_29D8;
    GsCOORDUNIT *G32 parts[3];
    s16 field_29E8, field_29EA;
    s32 sweep;
    s16 field_29F0, field_29F2;
    s32 field_29F4, field_29F8, phase;
    u8 unknown_2A00[8];
    s32 tint;
    s16 slot, command;
} Variant407EntryState;

extern u8 D_8013C8C8[];
void func_8013C2DC(u8 *context);
#endif
