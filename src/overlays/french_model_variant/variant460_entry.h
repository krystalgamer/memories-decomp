#ifndef MEMORIES_FRENCH_MODEL_VARIANT460_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT460_ENTRY_H
#include "../../types.h"
#include "variant337_entry.h"

typedef struct {
    CVECTOR streamer, outer, ribbon, sheet;
    u8 parts[8], unknown_18[8];
    s32 count, mode, kind;
    u32 sheet_start, ribbon_start, unknown_34, phase3, phase4;
} Variant460EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_238, field_23C, count, field_244, state, length;
    VECTOR velocity;
    u8 unknown_260[0x2E8 - 0x260];
} Variant460EntryRecord;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant460EntryRecord records[8];
    ModelVariantSheetSet sheets[16];
    u8 unknown_2100[0x23E8 - 0x2100];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_2C04[16];
    MATRIX matrix;
    SVECTOR target;
    MATRIX matrices[8];
    VECTOR targets[8];
    VECTOR directions[8];
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant460EntryConfig *G32 config;
    u32 field_2E80;
    GsCOORDUNIT *G32 parts[8];
    s16 field_2EA4, field_2EA6, field_2EA8, width;
    s32 field_2EAC, field_2EB0, field_2EB4, field_2EB8;
    s32 field_2EBC, field_2EC0, phase;
    u8 unknown_2EC8[8];
    s32 field_2ED0;
    s16 slot, command;
} Variant460EntryState;

extern u8 D_8013D8CC[];
void func_8013BCFC(u8 *context);
void func_8013C7EC(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
