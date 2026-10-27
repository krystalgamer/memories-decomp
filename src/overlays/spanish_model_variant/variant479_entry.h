#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../overlays/french_model_variant/variant379_grid.h"
#include "../model_variant/variant458_streamers.h"
#include "variant479_ribs.h"
#include "variant479_spiral.h"

/* One 0x2C-byte command record of header 479's table at D_8013ED20 + 0xFC. */
typedef struct {
    u8 unknown_00[4];
    u8 color[3];
    u8 unknown_07[9];
    u8 parts[3];
    u8 unknown_13[5];
    u32 start;
    u8 unknown_1C[0x10];
} Variant479EntryConfig;

/* Header 479's 0x4208-byte loader work area: the records of the accepted
 * helpers, their primitives, three part matrices and the shared timing. */
typedef struct {
    Grid379Sheet grids[5];
    Variant479Rib ribs[5];
    Variant479SpiralArm arms[12];
    ModelVariantSheet sheets[8];
    Variant458Streamer streamers[2];
    ModelVariantCurtain curtains[5];
    POLY_GT4 frame_poly, glow;
    u8 unknown_3EA0[0x34];
    POLY_GT4 strip;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    u8 unknown_4000[0x10];
    MATRIX matrices[3];
    SVECTOR target;
    VECTOR points[5];
    VECTOR directions[3];
    VECTOR deltas[5];
    u8 unknown_4148[4];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step;
    s32 fade;
    Variant479EntryConfig *G32 config;
    u32 field_417C;
    GsCOORDUNIT *G32 parts[3];
    s16 field_418C, field_418E;
    s32 field_4190, field_4194, field_4198, field_419C, field_41A0;
    s32 field_41A4, field_41A8, field_41AC, field_41B0, field_41B4;
    s16 field_41B8, field_41BA, field_41BC, field_41BE;
    s32 field_41C0, field_41C4, field_41C8, field_41CC;
    u8 color_a[4];
    u8 color_b[4];
    s32 field_41D8, field_41DC, field_41E0, field_41E4, field_41E8;
    s32 phase;
    s32 field_41F0, field_41F4;
    u8 unknown_41F8[8];
    s32 tint;
    s16 slot, command;
} Variant479EntryState;

extern u8 D_8013ED20[];
void func_8013BFD0(u8 *context);
void func_8013C4F8(u8 *context);
void func_8013CF68(u8 *context);
void func_8013DB90(u8 *context);
void func_8013E860(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
