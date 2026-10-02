#ifndef MEMORIES_FRENCH_MODEL_VARIANT422_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT422_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"
#include "../model_variant/variant405_halo.h"
#include "../model_variant/variant405_veils.h"

/* Only the CLUT halfwords are accessed; preceding stack bytes remain unassigned. */
typedef struct {
    u8 unknown_00[60];
    u16 texture_cluts[2];
} Variant422EntryStack;

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, end;
    u8 unknown_24[0xC];
} Variant422EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s32 field_19C;
    s16 field_1A0, field_1A2;
    u8 unknown_1A4[0x24];
} Variant422EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant422EntryQuad;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant405Veil veils[5];
    Variant405Halo halo;
    ModelVariantWebNarrow webs[3];
    Variant422EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant422EntryQuad quads[1];
    POLY_GT4 halo_poly;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1D3C[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant422EntryConfig *G32 config;
    u32 field_1DCC;
    GsCOORDUNIT *G32 parts[3];
    u8 unknown_1DDC[0x14];
    s32 field_1DF0, field_1DF4, field_1DF8, field_1DFC;
    s32 field_1E00, field_1E04, field_1E08;
    s16 field_1E0C, field_1E0E, field_1E10, field_1E12;
    s32 field_1E14, field_1E18, phase, field_1E20, field_1E24;
    u8 unknown_1E28[8];
    s32 tint;
    s16 slot, command;
} Variant422EntryState;

extern u8 D_8013E834[];
void func_8013C128(u8 *context);
void func_8013C620(u8 *context);
#endif
