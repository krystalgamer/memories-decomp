#ifndef MEMORIES_FRENCH_MODEL_VARIANT338_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT338_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR ribbon, inner, outer;
    u8 part, unknown_0D[5];
    u16 vertex;
    u8 unknown_14[8];
    s32 mode;
    u32 start;
    u8 unknown_24[16];
} Variant338EntryConfig;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant337EntryRecord records[5];
    Variant337EntryRing rings[2];
    u8 unknown_1364[0x167C - 0x1364];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    u8 unknown_1EB4[16];
    MATRIX matrix;
    VECTOR vertex_position;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant338EntryConfig *G32 config;
    u32 field_1F40;
    GsCOORDUNIT *G32 part;
    s16 field_1F48, field_1F4A, field_1F4C, width;
    s32 field_1F50, field_1F54, field_1F58, field_1F5C;
    s32 field_1F60, field_1F64, phase;
    u8 unknown_1F6C[8];
    s32 field_1F74;
    s16 slot, command;
} Variant338EntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant338EntryProjection;

extern u8 D_8013D70C[];
void func_8013BBA0(u8 *context);
void func_8013C6D8(u8 *context);
void func_8013CED4(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
