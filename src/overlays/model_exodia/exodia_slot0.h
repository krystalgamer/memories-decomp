#ifndef MODEL_EXODIA_SLOT0_H
#define MODEL_EXODIA_SLOT0_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/camera_view.h"

typedef struct {
    u8 prefix[28];
    u32 start, ramp_end, growth_end, shrink_end, end;
} ExodiaRingTiming;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale;
    u8 tail[12];
} ExodiaRing;

typedef struct {
    u8 prefix[0x1C0];
    ExodiaRing rings[1];
    u8 gap_258[0x2CC - 0x258];
    POLY_GT4 quad;
    u8 gap_300[0x3F0 - 0x300];
    VECTOR origin;
    u8 gap_400[0x450 - 0x400];
    u32 frame_count;
    u32 frame;
    u32 field_458;
    s32 step;
    u32 field_460;
    ExodiaRingTiming *timing;
    u8 gap_468[0x4DC - 0x468];
    s32 grown;
} ExodiaRingState;

typedef struct {
    u8 prefix[0x298];
    POLY_GT4 quad;
    u8 gap_2CC[0x3F0 - 0x2CC];
    VECTOR origin;
    u8 gap_400[0x438 - 0x400];
    VECTOR direction;
    u8 gap_448[8];
    u32 frame_count, frame;
    u8 gap_458[12];
    ExodiaRingTiming *timing;
    u8 gap_468[0x488 - 0x468];
    s32 scale, field_48C, width, angle;
} ExodiaSpokeState;

/* MODEL_EXODIA_USA selects the NTSC CLUT row. */
#ifdef MODEL_EXODIA_USA
#define EXODIA_ENTRY_CLUT_X 256
#define EXODIA_ENTRY_CLUT_Y 241
#else
#define EXODIA_ENTRY_CLUT_X 640
#define EXODIA_ENTRY_CLUT_Y 225
#endif

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8];
    u8 parts[3];
    u8 unknown_13[9];
    u32 start, ramp_end, growth_end, shrink_end, end;
    u8 tail[8];
} ExodiaEntryConfig;

typedef struct {
    CVECTOR inner[2], outer[2];
    u8 tail[12];
} ExodiaColorRecord;

typedef struct {
    ExodiaColorRecord colors[16];
    ModelVariantPointSheet rings[1];
    ModelVariantPackets packets;
    u8 gap_300[0x3DC - 0x300];
    MATRIX matrix;
    SVECTOR target;
    VECTOR position, delta, remaining;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step;
    u32 field_460;
    ExodiaEntryConfig *config;
    u32 field_468;
    GsCOORDUNIT *parts[3];
    s16 field_478, field_47A, field_47C, field_47E;
    s32 field_480, progress, spoke_scale, field_48C, spoke_width, spoke_angle;
    s32 field_498, field_49C, field_4A0, field_4A4, field_4A8, field_4AC;
    s32 field_4B0, field_4B4, field_4B8, field_4BC, field_4C0, field_4C4;
    s32 field_4C8, field_4CC, field_4D0, field_4D4, field_4D8;
    s32 grown, field_4E0, field_4E4, field_4E8, field_4EC;
    s16 slot, command;
} ExodiaEntryState;

extern ExodiaEntryConfig gModelExodia_aConfigs[];
s32 ModelExodia_HandleRequest(SVECTOR *point, s32 command);
void ModelExodia_DrawRing(u8 *context);
void ModelExodia_DrawSpokes(u8 *context);
#endif
