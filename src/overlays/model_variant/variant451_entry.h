#ifndef MEMORIES_MODEL_VARIANT451_ENTRY_H
#define MEMORIES_MODEL_VARIANT451_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "variant412_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 count;
    s32 mode;
    u32 rate;
    u32 start;
    u32 limit;
    u8 unknown_30[8];
    u32 origin_end;
    u32 sweep_start, sweep_end;
} Variant451EntryConfig;

/* One group of 16 sprites at the start of the work area, drawn by
 * FrenchModelVariant468_DrawSwarm (image134622_swarm.c). */
typedef struct {
    SVECTOR v0[16], v1[16], v2[16], v3[16], origin[16];
    CVECTOR color;
    u8 unknown_284[0x10];
    s32 scale[16];
    s32 field_2D4[16];
    s32 field_314[16];
    s32 done[16];
    s32 fade[16];
    MATRIX origins[16];
} Variant451EntryGroup;

typedef struct {
    Variant451EntryGroup group;
    Variant412EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant412EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    ModelVariantFan quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_1E88[0x24];
    MATRIX matrix;
    SVECTOR target;
    SVECTOR center;
    VECTOR direction;
    VECTOR drift;
    DVECTOR screen_delta;
    VECTOR positions[16];
    VECTOR offsets[16];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant451EntryConfig *config;
    u32 field_2130;
    GsCOORDUNIT *parts[3];
    s16 field_2140, field_2142;
    s32 sweep;
    s32 spin, drift_scale, field_2150;
    s16 field_2154, field_2156;
    s32 field_2158, field_215C, phase;
    u8 unknown_2164[8];
    s32 tint;
    s16 slot, command;
} Variant451EntryState;

extern GsIMAGE gModelVariant451_aImages[9];
extern Variant451EntryConfig gModelVariant451_aConfigs[];
void FrenchModelVariant468_DrawSwarm(u8 *context);
#endif
