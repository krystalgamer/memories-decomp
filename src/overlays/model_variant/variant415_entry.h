#ifndef MEMORIES_DECOMP_MODEL_VARIANT415_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT415_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    u8 unknown_00[4], parts[5], unknown_09[11];
    u32 timings[15];
} Variant415EntryConfig;

/* Two ends of three ribbons: the points, their screen positions, a colour,
 * a scale and the sweep amount of each end. */
typedef struct {
    SVECTOR point[3][2];
    PSXLONG screen[3][2];
    CVECTOR color;
    VECTOR scale;
    s16 field_5C, field_5E;
    s32 field_60;
    s32 amount[2];
    s32 otz[2];
} Variant415EntryRibbon;

/* Two seventeen-point loops with a colour pair, a scale and a done flag. */
typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    CVECTOR color[2];
    s32 scale;
    s32 done;
} Variant415EntryStrand;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[2];
    VECTOR scale;
    u8 unknown_60[0x24];
} Variant415EntryLine;

typedef struct {
    u8 unknown_000[0x81A];
    s16 scale;
    u8 unknown_81C[0x200];
} Variant415EntrySweep;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant415EntryRibbon ribbons[3];
    ModelVariantSheet sheets[2];
    Variant415EntryStrand strands_a[3];
    Variant415EntryStrand strands_b[3];
    ModelVariantRing rings[4];
    Variant415EntryLine lines[2];
    Variant415EntrySweep sweeps[3];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 bars[16];
    POLY_FT4 flat_textured[2];
    u8 unknown_2F20[0x44];
    SVECTOR target;
    u8 unknown_2F6C[0x10];
    s16 field_2F7C, field_2F7E;
    MATRIX matrices[5];
    VECTOR directions[5];
    s16 screen_x[5], screen_y[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant415EntryConfig *G32 config;
    u32 field_30B4;
    GsCOORDUNIT *G32 parts[5];
    s32 field_30CC, field_30D0, field_30D4, start_index, field_30DC, field_30E0;
    u8 unknown_30E4[4];
    s32 phase;
    u8 unknown_30EC[8];
    s32 tint;
    s16 slot, command;
} Variant415EntryState;

extern u8 gModelVariant415_D1F7C[];
void ModelVariant415_DrawRibbons(Variant415EntryRibbon *ribbon);
void SpanishModelVariant432_DrawSheets(u8 *context, s32 phase);
void SpanishModelVariant432_DrawBandLayers(u8 *context);
void SpanishModelVariant432_DrawBands(u8 *context);
typedef struct {
    u8 unknown_00[60];
    u16 texture_cluts[2];
} Variant415EntryStack;
#endif
