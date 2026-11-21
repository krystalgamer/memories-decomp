#ifndef MODEL_EXODIA_SLOT1_H
#define MODEL_EXODIA_SLOT1_H
#include "../../types.h"
#include "exodia_slot0.h"
#include "e530_spiral.h"
#include "../../game/model_view_coordinate_helpers.h"
#include "../../game/gpu_packets.h"

/* MODEL_EXODIA_USA selects the NTSC CLUT rows and screen height. */
#ifdef MODEL_EXODIA_USA
#define EXODIA_SECOND_CLUT_X 256
#define EXODIA_SECOND_CLUT_Y 241
#define EXODIA_SECOND_GLOW_CLUT_X 512
#define EXODIA_SECOND_SCREEN_HEIGHT 240
#else
#define EXODIA_SECOND_CLUT_X 640
#define EXODIA_SECOND_CLUT_Y 225
#define EXODIA_SECOND_GLOW_CLUT_X 640
#define EXODIA_SECOND_SCREEN_HEIGHT 256
#endif

typedef struct {
    u8 unknown_00[4];
    u8 parts[3];
    u8 unknown_07[9];
    u32 start, ramp_end, petals_start;
    u32 pulse0, pulse1, pulse2, pulse3, pulse4, pulse5, pulse6;
    u32 fade_start, fade_end, end;
} ExodiaSecondEntryConfig;

/* One beam: outer, centre and inner rows of seventeen points, their
 * projections, the colour, and per point the projection flag and depth. */
typedef struct {
    SVECTOR outer[17];
    SVECTOR center[17];
    SVECTOR inner[17];
    s32 projected[3][17];
    CVECTOR color;
    s32 field_268, field_26C, field_270;
    u8 unknown_274[4];
    s16 field_278, field_27A;
    s32 field_27C;
    s32 flags[17];
    s32 depth[17];
} ExodiaSecondEntryBeam;

typedef struct {
    ExodiaSecondEntryBeam beams[1];
    ModelVariantPointSheet rings[2];
    ModelVariantFlaggedSpiralArm petals[12];
    POLY_F4 flat;
    ModelVariantPackets packets;
    u8 gap_B28[0xC04 - 0xB28];
    MATRIX matrix;
    SVECTOR target, origin;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    ExodiaSecondEntryConfig *config;
    u32 field_C78;
    GsCOORDUNIT *parts[3];
    u32 field_C88;
    ModelVariantSpiralParams petal;
    s16 field_C94, field_C96;
    s16 distance, width;
    s32 field_C9C, field_CA0, phase, field_CA8, field_CAC, field_CB0, field_CB4;
    s16 slot, command;
} ExodiaSecondEntryState;

extern ExodiaSecondEntryConfig gModelExodia_aSecondConfigs[];
s32 ModelExodia_HandleSecondRequest(ExodiaSecondEntryState *point, s32 command);
void ModelExodia_DrawRings(ExodiaSecondEntryState *context);
void ModelExodia_DrawPetals(ExodiaSecondEntryState *context);
void ModelExodia_DrawBeam(ExodiaSecondEntryState *context);
#endif
