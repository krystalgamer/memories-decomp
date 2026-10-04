#ifndef MEMORIES_MODEL_VARIANT404_ENTRY_H
#define MEMORIES_MODEL_VARIANT404_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 mode;
    u32 start;
    u8 unknown_24[0x10];
    s16 size;
    u8 unknown_36[6];
} Variant404EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} Variant404EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant404EntryQuad;

/* 0x7C-byte record at work + 0x4E0 used by func_8013C864. */
typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} Variant404EntryGlow;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant404EntryGlow glows[12];
    u8 unknown_AB0[0x360];
    Variant404EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant404EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1864[0x24];
    MATRIX matrix, matrix0, matrix1;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_1930, field_1932, field_1934, field_1936;
    Variant404EntryConfig *G32 config;
    u32 field_193C;
    GsCOORDUNIT *G32 parts[3];
    s16 field_194C, field_194E, field_1950, field_1952;
    s32 field_1954, field_1958, field_195C, phase, field_1964;
    u8 unknown_1968[8];
    s32 tint;
    s16 slot, command;
} Variant404EntryState;

extern u8 D_8013F0FC[];
void func_8013C1D4(u8 *context);
void func_8013C864(u8 *context);
void func_8013D4F8(u8 *context);
void func_8013DCA4(u8 *context);
void func_8013E18C(u8 *context);
#endif
