#ifndef MEMORIES_MODEL_VARIANT459_ENTRY_H
#define MEMORIES_MODEL_VARIANT459_ENTRY_H
#include "../../types.h"
#include "../model_variant/e503_h320_entry_pal.h"
#include "../../game/model_camera_effects.h"
#include "../model_variant/h459_screen_grid.h"
#include "h459_curtains.h"
#include "h459_grid.h"

/* The config entries are 0x38 bytes: three colours, the parts, the start and
 * second frames, the window over which the sheet and spiral arms shrink, the
 * fade window, and the frame the globe enters phase 4 and the curtains stop
 * cycling. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, second;
    u32 shrink_start;
    u32 shrink_end;
    u32 fade_start, fade_end;
    u32 phase4_frame;
} Variant459EntryConfig;

typedef struct {
    ModelVariantWeb webs[3];
    ModelVariantFlaggedSpiralArm glows[16];
    ModelVariantSheet sheet;
    ModelVariantMesh grid;
    Variant476ScreenGrid screen_grid;
    Variant459Curtain curtains[5];
    ModelVariantPackets packets;
    POLY_FT4 flat_textured[2];
    u8 unknown_236C[0x10];
    GsGLINE line;
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
    ModelVariantClock clock;
    Variant459EntryConfig *config;
    u32 field_27C4;
    GsCOORDUNIT *parts[3];
    s16 field_27D4, field_27D6, field_27D8, field_27DA;
    s32 field_27DC, time;
    s32 spiral_size, field_27E8, spiral_arm_length, spiral_angle, field_27F4, field_27F8;
    s32 globe_scale, field_2800, globe_rotation_x, globe_pulse_angle, globe_brightness;
    s32 field_2810, field_2814, field_2818;
    s32 field_281C, field_2820, field_2824, screen_grid_brightness, field_282C, field_2830;
    s32 facing;
    s32 zeroes[5];
    s32 phase, field_2850;
    u8 unknown_2854[8];
    s32 tint;
    s16 slot, command;
} Variant459EntryState;

void ModelVariant459_DrawSheet(Variant459EntryState *context);
void FrenchModelVariant476_DrawSpiral(Variant459EntryState *context);
void ModelVariant459_DrawCurtains(Variant459EntryState *context);
void ModelVariant459_DrawGlobe(Variant459EntryState *context);
extern GsIMAGE gModelVariant459_aImages[9];
extern Variant459EntryConfig gModelVariant459_aConfigs[];
void FrenchModelVariant476_DrawScreenGrid(Variant459EntryState *context);

#endif
