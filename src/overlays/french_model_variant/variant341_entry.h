#ifndef MEMORIES_FRENCH_MODEL_VARIANT341_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT341_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"
#include "../model_variant/variant324_fan.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 substeps;
    u8 unknown_0E[2];
    u32 duration;
} Variant341EntryConfig;

typedef struct {
    u8 unknown_00[0x144];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x48];
} Variant341EntryBand;

/* Minimum entry-observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant341EntryBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant324Fan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_EA0[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame, step, fade;
    Variant341EntryConfig *G32 config;
    u32 field_F30;
    GsCOORDUNIT *G32 parts[3];
    s16 substep, field_F42, field_F44, field_F46;
    s32 field_F48, field_F4C, phase;
    u8 unknown_F54[8];
    s32 tint;
    s16 slot, command;
} Variant341EntryState;

extern u8 D_8013DD58[];
void func_8013BF38(u8 *context);
void func_8013C354(u8 *context);
void func_8013C7C0(u8 *context);
void func_8013D2F8(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
