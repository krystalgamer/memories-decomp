#ifndef MEMORIES_FRENCH_MODEL_VARIANT336_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT336_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    u8 unknown_00[4];
    u8 part0, part1, unknown_06[10];
    u32 start, end;
    u8 unknown_18[12];
} Variant336EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x2E8 - 0x240];
    s32 state, count, extent;
    u8 unknown_2F4[0x3C0 - 0x2F4];
} Variant336EntryRecord;

typedef struct {
    SVECTOR points[16];
    CVECTOR outer, inner;
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} Variant336EntrySheet;

/* Only the entry's measured view, not an allocation-capacity claim. */
typedef struct {
    Variant336EntryRecord records[4];
    Variant336EntrySheet sheets[8];
    Variant337EntryStreamer streamers[2];
    u8 unknown_1AB0[0x1DC8 - 0x1AB0];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    u8 unknown_1F10[0x1F34 - 0x1F10];
    s32 origin[3];
    MATRIX matrix0, matrix1;
    u8 unknown_1F80[16];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant336EntryConfig *G32 config;
    u8 unknown_1FDC[4];
    GsCOORDUNIT *G32 part0, *G32 part1;
    s32 field_1FE8, sheet_count;
    s16 field_1FF0, field_1FF2;
    s32 field_1FF4, field_1FF8, extent, phase;
    u8 unknown_2004[8];
    s32 brightness;
    s16 slot, command;
    s32 rotation[3];
} Variant336EntryState;

extern u8 D_8013D3B8[];
s32 func_8013B004(SVECTOR *point, s32 command);
void func_8013BA9C(u8 *context);
void func_8013C3D4(u8 *context);
void func_8013CB6C(u8 *context);
#endif
