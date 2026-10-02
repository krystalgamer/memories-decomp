#ifndef FRENCH_SPECIAL_SECOND_ENTRY_H
#define FRENCH_SPECIAL_SECOND_ENTRY_H
#include "../../types.h"
#include "ring_second.h"
#include "beam.h"
#include "petals.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/camera_view.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"

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
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x24];
} ExodiaSecondEntryPetal;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale, field_8C, field_90, field_94;
} ExodiaSecondEntryRing;

typedef struct {
    ExodiaSecondEntryBeam beams[1];
    ExodiaSecondEntryRing rings[2];
    ExodiaSecondEntryPetal petals[12];
    POLY_F4 flat;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 gap_B28[0xC04 - 0xB28];
    MATRIX matrix;
    SVECTOR target, origin;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    ExodiaSecondEntryConfig *G32 config;
    u32 field_C78;
    GsCOORDUNIT *G32 parts[3];
    u32 field_C88;
    s16 field_C8C, field_C8E, field_C90, field_C92, field_C94, field_C96;
    s16 distance, width;
    s32 field_C9C, field_CA0, phase, field_CA8, field_CAC, field_CB0, field_CB4;
    s16 slot, command;
} ExodiaSecondEntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} ExodiaSecondEntryProjection;

extern ExodiaSecondEntryConfig D_8017CD3C[];
void func_80059B90(s16 value, s16 *out);
s32 func_8017B004(SVECTOR *point, s32 command);
#endif
