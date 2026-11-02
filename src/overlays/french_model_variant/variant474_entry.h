#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT474_ENTRY_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT474_ENTRY_H
#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "variant337_entry.h"

/* One 0x34-byte command row: the outer and inner sheet colours, the model
 * part, the trigger frame and the fade window. */
typedef struct {
    u8 color[3], pad03;
    u8 secondary[3], pad07;
    u8 unknown_08[8];
    u8 part, unknown_11[7];
    u32 trigger;
    u8 unknown_1C[4];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_28[12];
} Variant474EntryConfig;

/* The ribbon view the header-474 ribbons helper draws: phases, amplitudes,
 * two colour rows, a scale and a heading. */
typedef struct {
    u8 unknown_000[0x220];
    s32 phase[17];
    s32 amp[17];
    CVECTOR color[17];
    CVECTOR secondary[17];
    s32 scale[3];
    u8 unknown_33C[4];
    s16 field_340, field_342;
    s32 field_344;
    u8 unknown_348[0x88];
    s32 heading[3];
    u8 unknown_3DC[0xD0];
} Variant474EntryRibbon;

/* One funnel: seventeen inner and outer ring points and a size. */
typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    s32 field_114;
} Variant474EntryFunnel;

/* One 0x7C-byte spiral record: two inner and two outer colours. */
typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} Variant474EntrySpiral;

/* One 0x98-byte sheet: four quads given by corner arrays and two colours. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} Variant474EntrySheet;

/* One 0x334-byte streamer: seventeen colours. */
typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} Variant474EntryStreamer;

/* The 9x17 mesh points and nine row colours. */
typedef struct {
    SVECTOR row[9][17];
    u8 color[9][4];
} Variant474EntryMesh;

/* The header-474 work area shared with its helpers. */
typedef struct {
    Variant474EntryFunnel funnels[2];
    Variant474EntryRibbon ribbon;
    Variant474EntryMesh mesh;
    Variant474EntrySpiral spirals[12];
    Variant474EntrySheet sheets[2];
    Variant474EntryStreamer streamers[2];
    u8 unknown_1930[0x34];
    POLY_GT4 glow;
    u8 unknown_1998[0x34];
    POLY_GT4 bars[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    u8 unknown_1B2C[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    s16 screen_x, screen_y;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant474EntryConfig *G32 config;
    u8 unknown_1BA8[4];
    GsCOORDUNIT *G32 part;
    s16 field_1BB0, field_1BB2;
    s32 field_1BB4, field_1BB8, field_1BBC, field_1BC0, field_1BC4, field_1BC8;
    s32 field_1BCC, field_1BD0, field_1BD4, field_1BD8, field_1BDC;
    s32 field_1BE0, field_1BE4;
    s16 field_1BE8, field_1BEA, field_1BEC, field_1BEE;
    s32 field_1BF0, field_1BF4, field_1BF8, field_1BFC;
    CVECTOR inner_color, outer_color;
    s32 field_1C08, field_1C0C, field_1C10, field_1C14, field_1C18;
    s32 phase;
    s32 field_1C20, field_1C24;
    SVECTOR camera;
    s32 tint;
    s16 slot, command;
} Variant474EntryState;

extern u8 gFrenchModelVariant474_D3980[];
void FrenchModelVariant474_DrawRibbon(u8 *context);
void FrenchModelVariant474_DrawPairedFunnels(u8 *context);
#endif
