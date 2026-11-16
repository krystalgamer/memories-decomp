#ifndef MODEL_EXODIA_SLOT1_H
#define MODEL_EXODIA_SLOT1_H
#include "../../types.h"
#include "exodia_slot0.h"
#include "../model_variant/variant418_spiral.h"
#include "../../game/model_view_coordinate_helpers.h"
#include "../../game/gpu_packets.h"

typedef struct {
    u8 prefix[16];
    u32 start, ramp_end, field_18, second_start;
} ExodiaSecondRingTiming;

typedef struct {
    u8 prefix[0x308];
    ExodiaRing rings[2];
    u8 gap_438[0xAF4 - 0x438];
    POLY_GT4 quad;
    u8 gap_B28[0xC2C - 0xB28];
    SVECTOR origin;
    VECTOR direction;
    u8 gap_C44[0xC60 - 0xC44];
    u32 frame_count;
    u32 frame;
    u8 gap_C68[0xC74 - 0xC68];
    ExodiaSecondRingTiming *timing;
    u8 gap_C78[0xC98 - 0xC78];
    s16 distance;
    u8 gap_C9A[0xCA4 - 0xC9A];
    s32 phase;
} ExodiaSecondRingState;

typedef struct {
    SVECTOR outer[17];
    SVECTOR center[17];
    SVECTOR inner[17];
    s32 projected[3][17];
    u8 gap_264[0x280 - 0x264];
    s32 flags[17];
    s32 depth[17];
} ExodiaBeam;

typedef struct {
    ExodiaBeam beams[1];
    u8 gap_308[0xAC0 - 0x308];
    POLY_GT4 quad;
    u8 gap_AF4[0xC2C - 0xAF4];
    SVECTOR origin;
    VECTOR direction;
    SVECTOR angles;
    u8 gap_C4C[0xC60 - 0xC4C];
    u32 frame_count;
    u32 frame;
    u32 field_C68;
    u32 step;
    u8 gap_C70[0xC98 - 0xC70];
    s16 distance;
    s16 width;
    u8 gap_C9C[0xCA4 - 0xC9C];
    s32 phase;
} ExodiaBeamState;

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

typedef struct {
    u8 unknown_00[0x264];
    CVECTOR color;
    s32 field_268, field_26C, field_270;
    u8 unknown_274[4];
    s16 field_278, field_27A;
    s32 field_27C;
    u8 unknown_280[0x88];
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
    s16 petal_angle, field_C8E, petal_radius, field_C92, field_C94, field_C96;
    s16 distance, width;
    s32 field_C9C, field_CA0, phase, field_CA8, field_CAC, field_CB0, field_CB4;
    s16 slot, command;
} ExodiaSecondEntryState;

extern ExodiaSecondEntryConfig gModelExodia_aSecondConfigs[];
s32 ModelExodia_HandleSecondRequest(SVECTOR *point, s32 command);
void ModelExodia_DrawRings(u8 *context);
void ModelExodia_DrawPetals(u8 *context);
void ModelExodia_DrawBeam(u8 *context);
#endif
