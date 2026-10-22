#ifndef MEMORIES_MODEL_VARIANT425_ENTRY_H
#define MEMORIES_MODEL_VARIANT425_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* The entry's views of the spiral arm and ray records (the same layouts as
 * Variant425SpiralArm and Variant425Ray), with the two or three colour rows
 * the entry fills named. */
typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 cb[2][4];
    u8 ca[2][4];
    u8 unknown60[4];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} Variant425EntryArm;

typedef struct {
    u8 unknown00[0x18];
    SVECTOR a[3];
    PSXLONG sa[3];
    s32 angle[3];
    SVECTOR b[3];
    PSXLONG sb[3];
    s32 width[3];
    u8 cb[3][4];
    u8 ca[3][4];
    u8 unknown90[4];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    u8 unknownAC[0xC];
} Variant425EntryRay;

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x18];
} Variant425EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} Variant425EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant425EntryFan;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebSmall webs[3];
    Variant425EntryArm arms[16];
    Variant425EntryRay rays[16];
    u8 unknown_1940[0x1CA0 - 0x1940];
    Variant425EntryBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant425EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_265C[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR orbit;
    VECTOR direction;
    VECTOR offset;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_2708, field_270A, field_270C, field_270E;
    s32 field_2710;
    Variant425EntryConfig *G32 config;
    u32 field_2718;
    GsCOORDUNIT *G32 parts[3];
    s16 field_2728, field_272A, field_272C, field_272E;
    s32 field_2730, sweep, field_2738, field_273C, field_2740, field_2744;
    s32 phase, field_274C;
    u8 unknown_2750[8];
    s32 tint;
    s16 slot, command;
} Variant425EntryState;

extern u8 D_8013F3FC[];
void func_8013C178(u8 *context);
void func_8013C568(u8 *context);
void func_8013CF14(u8 *context);
void func_8013D8F8(u8 *context);
#endif
