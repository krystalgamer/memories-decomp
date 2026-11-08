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

/* The header-474 work area shared with its helpers. */
typedef struct {
    ModelVariantCurtain funnels[2];
    Variant474EntryRibbon ribbon;
    ModelVariantMesh mesh;
    ModelVariantSpiral spirals[12];
    ModelVariantSheet sheets[2];
    ModelVariantStreamer streamers[2];
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
    Variant474EntryConfig *config;
    u8 unknown_1BA8[4];
    GsCOORDUNIT *part;
    s16 field_1BB0, field_1BB2;
    /* The mesh helper scales the mesh by mesh_size and its colours by
     * mesh_intensity / 1024; the sheet helper moves the second sheet
     * displacement / 1024 along the direction. */
    s32 width, ribbon_angle, scale, field_1BC0, mesh_size, field_1BC8;
    s32 field_1BCC, field_1BD0, mesh_intensity, field_1BD8, field_1BDC;
    s32 displacement, progress;
    s16 spiral_angle, field_1BEA, spiral_size, field_1BEE;
    ModelVariantStreamerParams streamer;
    CVECTOR inner_color, outer_color;
    s32 spin, field_1C0C, field_1C10, field_1C14, field_1C18;
    s32 phase;
    s32 field_1C20, field_1C24;
    SVECTOR camera;
    s32 tint;
    s16 slot, command;
} Variant474EntryState;

extern GsIMAGE gFrenchModelVariant474_aImages[8];
extern Variant474EntryConfig gFrenchModelVariant474_aConfigs[];
void FrenchModelVariant474_DrawRibbon(u8 *context);
void FrenchModelVariant474_DrawPairedFunnels(u8 *context);
#endif
