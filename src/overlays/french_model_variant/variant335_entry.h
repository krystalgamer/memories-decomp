#ifndef FRENCH335_ENTRY_VIEW_H
#define FRENCH335_ENTRY_VIEW_H
#include "../../types.h"
#include "variant337_entry.h"
#include "variant335_rings.h"

typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 start;
    u8 unknown_10[0x1C];
    u32 end;
} Model335EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x394 - 0x240];
} Model335EntryRecord;

typedef struct {
    SVECTOR points[16];
    CVECTOR outer, inner;
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} Model335EntrySheet;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x334 - 0x2AC];
} Model335EntryShortStreamer;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Model335EntryRecord records[3];
    Model335EntrySheet sheets[4];
    Variant335Ring rings[3];
    Model335EntryShortStreamer short_streamers[6];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[3];
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    u8 unknown_2DB8[16];
    MATRIX matrix;
    VECTOR origin, end, delta;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Model335EntryConfig *G32 config;
    u8 unknown_2E64[4];
    GsCOORDUNIT *G32 part;
    s32 field_2E6C, field_2E70, field_2E74, field_2E78;
    s32 rotation[3], extent;
    s16 count, field_2E8E, field_2E90, width, field_2E94, field_2E96;
    s32 wave0, wave1, phase;
    u8 unknown_2EA4[8];
    s32 brightness;
    s16 slot, command;
} Model335EntryState;

s32 func_8013B004(SVECTOR *point, s32 command);
extern u8 D_8013D8EC[];
void func_8013BB98(u8 *context);
void func_8013C534(u8 *context);
void func_8013CBC4(u8 *context);
void func_8013D0D8(u8 *context);
#endif
