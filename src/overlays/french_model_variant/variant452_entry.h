#ifndef MEMORIES_FRENCH_MODEL_VARIANT452_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT452_ENTRY_H
#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "variant439_entry.h"
#include "variant452_streamers.h"

/* One 0x38-byte row of header 452's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u8 unknown_20[0x18];
} Variant452EntryConfig;

/* One 0xB8-byte record whose three colour pairs the entry seeds. */
typedef struct {
    u8 unknown_00[0x78];
    CVECTOR outer[3], inner[3];
    u8 unknown_90[0x28];
} Variant452EntryTrail;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWeb webs[3];
    Variant452Streamer streamers[16];
    Variant452EntryTrail trails[16];
    u8 unknown_25A0[0x360];
    Variant439EntryBand bands[1];
    ModelVariantSheet sheets[1];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant439EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_32BC[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR position, delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    s16 field_3368, field_336A, field_336C, field_336E;
    s32 field_3370;
    Variant452EntryConfig *G32 config;
    u32 field_3378;
    GsCOORDUNIT *G32 parts[3];
    s16 field_3388, field_338A, field_338C, field_338E;
    s32 field_3390, progress, field_3398, field_339C, field_33A0, field_33A4;
    s32 phase, field_33AC;
    u8 unknown_33B0[8];
    s32 tint;
    s16 slot, command;
} Variant452EntryState;

extern u8 D_8013D3E4[];
void func_8013C0E0(u8 *context);
void func_8013C51C(u8 *context);
void func_8013D010(u8 *context);
s32 func_8013B004(SVECTOR *point, s32 command);
#endif
