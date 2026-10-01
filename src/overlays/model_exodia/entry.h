#ifndef FRENCH_SPECIAL_ENTRY_H
#define FRENCH_SPECIAL_ENTRY_H
#include "../../types.h"
#include "ring.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

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
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale, field_8C, field_90, field_94;
} ExodiaEntryRing;

typedef struct {
    ExodiaColorRecord colors[16];
    ExodiaEntryRing rings[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
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
    ExodiaEntryConfig *G32 config;
    u32 field_468;
    GsCOORDUNIT *G32 parts[3];
    s16 field_478, field_47A, field_47C, field_47E;
    s32 field_480, progress, field_488, field_48C, field_490, field_494;
    s32 field_498, field_49C, field_4A0, field_4A4, field_4A8, field_4AC;
    s32 field_4B0, field_4B4, field_4B8, field_4BC, field_4C0, field_4C4;
    s32 field_4C8, field_4CC, field_4D0, field_4D4, field_4D8;
    s32 grown, field_4E0, field_4E4, field_4E8, field_4EC;
    s16 slot, command;
} ExodiaEntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} ExodiaEntryProjection;

extern ExodiaEntryConfig D_8013C7AC[];
void func_8013BC0C(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
