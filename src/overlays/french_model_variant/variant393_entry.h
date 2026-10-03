#ifndef MEMORIES_FRENCH_MODEL_VARIANT393_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT393_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR inner, ribbon, outer;
    u8 parts[3], unknown_0F;
    u16 vertices[3];
    u8 unknown_16[6];
    u16 count, unknown_1E;
    s32 mode;
    u32 start, end, field_2C, fade_start, fade_end;
    s32 offset[3];
} Variant393EntryConfig;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant337EntryRecord records[1];
    Variant337EntryRing rings[2];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2], extra_flat[2];
    u8 unknown_D0C[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR positions[3], direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant393EntryConfig *G32 config;
    u32 unknown_DB8;
    GsCOORDUNIT *G32 parts[3];
    s32 part_index;
    s16 field_DCC, field_DCE, field_DD0, width;
    s32 field_DD4, field_DD8, field_DDC, field_DE0, field_DE4;
    s32 field_DE8, field_DEC, field_DF0, field_DF4, phase;
    u8 unknown_DFC[8];
    s32 tint;
    s16 slot, command;
} Variant393EntryState;

typedef Variant337EntryProjection Variant393EntryProjection;

extern u8 D_8013D3B0[];
void func_8013BDDC(u8 *context);
void func_8013C704(u8 *context);
void func_8013CB8C(u8 *context);
typedef char Variant393EntryConfigSize[(sizeof(Variant393EntryConfig) == 0x44) ? 1 : -1];
typedef char Variant393EntryStateSize[(sizeof(Variant393EntryState) == 0xE0C) ? 1 : -1];
#endif
