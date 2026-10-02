#ifndef MEMORIES_FRENCH_MODEL_VARIANT431_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT431_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"
#include "../model_variant/variant414_fan.h"

typedef struct {
    u8 unknown_00[4], parts[5], unknown_09[11];
    u32 first_start;
    u8 unknown_18[0x14];
    u32 timings[15];
} Variant431EntryConfig;

typedef struct {
    u8 unknown_00[0xB4];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0, progress[5], field_108;
    u8 unknown_10C[0x14];
} Variant431EntryBand;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant431EntryBand bands[5];
    ModelVariantSheet sheets[6];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant414Fan fans[5];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1720[0x38];
    s32 position[3];
    SVECTOR target;
    u8 unknown_176C[0x14];
    MATRIX matrices[5];
    VECTOR directions[5];
    s16 screen_x[5], screen_y[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step;
    u32 fade;
    Variant431EntryConfig *G32 config;
    u32 field_18B4;
    GsCOORDUNIT *G32 parts[5];
    s16 field_18CC, field_18CE, field_18D0, field_18D2;
    s32 field_18D4, start_index, field_18DC, phase;
    u8 unknown_18E4[8];
    s32 tint;
    s16 slot, command;
} Variant431EntryState;

extern u8 D_8013DF48[];
void func_8013BF1C(u8 *context);
void func_8013C338(u8 *context);
void func_8013C7C8(u8 *context);
void func_8013D444(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
