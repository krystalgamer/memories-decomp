#ifndef MEMORIES_MODEL_VARIANT418_ENTRY_H
#define MEMORIES_MODEL_VARIANT418_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant418_spiral.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13;
    u16 vertex;
    u8 unknown_16[0x0E];
    s32 mode;
    u32 start;
    u8 unknown_2C[0x10];
    u32 end;
    u8 unknown_40[4];
} Variant418EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x48];
} Variant418EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant418EntryFan;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWeb webs[3];
    Variant418SpiralArm arms[12];
    u8 unknown_D50[0x10F0 - 0xD50];
    Variant418EntryBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant418EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1AD0[0x24];
    MATRIX matrix;
    VECTOR vertex_position;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_1B6C, field_1B6E, field_1B70, field_1B72;
    Variant418EntryConfig *G32 config;
    u32 field_1B78;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1B88, field_1B8A, field_1B8C, field_1B8E;
    s32 field_1B90, field_1B94, field_1B98;
    s32 phase, field_1BA0;
    u8 unknown_1BA4[8];
    s32 tint;
    s16 slot, command;
} Variant418EntryState;

extern u8 D_8013E96C[];
void func_8013C088(u8 *context);
void func_8013CAA4(u8 *context);
void func_8013CE50(u8 *context);
#endif
