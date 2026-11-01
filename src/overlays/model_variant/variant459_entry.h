#ifndef MEMORIES_MODEL_VARIANT459_ENTRY_H
#define MEMORIES_MODEL_VARIANT459_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_effect_state.h"
#include "../french_model_variant/variant476_screen_grid.h"
#include "variant459_curtains.h"
#include "variant459_grid.h"

/* The config entries are 0x38 bytes: three colours, the parts, the start and
 * second frames and the fade window. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, second;
    u8 unknown_24[8];
    u32 fade_start, fade_end;
    u8 unknown_34[4];
} Variant459EntryConfig;

typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x24];
} Variant459EntryGlow;

typedef struct {
    ModelVariantWeb webs[3];
    Variant459EntryGlow glows[16];
    ModelVariantSheet sheet;
    Variant459Grid grid;
    Variant476ScreenGrid screen_grid;
    Variant459Curtain curtains[5];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    u8 unknown_236C[0x24];
    POLY_GT4 plain;
    u8 unknown_23C4[0x30C];
    POLY_GT4 veil, ray;
    MATRIX matrix;
    SVECTOR target;
    VECTOR position;
    VECTOR direction;
    VECTOR offset;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant459EntryConfig *G32 config;
    u32 field_27C4;
    GsCOORDUNIT *G32 parts[3];
    s16 field_27D4, field_27D6, field_27D8, field_27DA;
    s32 field_27DC, time;
    s32 field_27E4, field_27E8, field_27EC, field_27F0, field_27F4, field_27F8;
    s32 field_27FC, field_2800, field_2804, field_2808, field_280C, field_2810, field_2814, field_2818;
    s32 field_281C, field_2820, field_2824, field_2828, field_282C, field_2830;
    s32 facing;
    s32 zeroes[5];
    s32 phase, field_2850;
    u8 unknown_2854[8];
    s32 tint;
    s16 slot, command;
} Variant459EntryState;

void ModelVariant459_DrawSheet(u8 *context);
void FrenchModelVariant476_DrawSpiral(u8 *context);
void ModelVariant459_DrawCurtains(u8 *context);
void ModelVariant459_DrawGlobe(u8 *context);
extern u8 gModelVariant459_D3338[];
void FrenchModelVariant476_DrawScreenGrid(u8 *context);

#endif
