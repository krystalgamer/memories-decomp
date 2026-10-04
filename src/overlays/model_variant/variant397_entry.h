#ifndef MEMORIES_MODEL_VARIANT397_ENTRY_H
#define MEMORIES_MODEL_VARIANT397_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/func_80058E1C.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4];
    u32 linked;
    u8 parts[3], unknown_17[9];
    u32 start, field_24, field_28, field_2C, field_30;
    s32 offset[3];
    u16 extent, field_42;
    s32 field_44;
} Variant397EntryConfig;

typedef struct {
    SVECTOR a[5], b[5], c[5];
    PSXLONG sa[5], sb[5], sc[5];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0;
    s32 otz[5];
    PSXLONG flag[5];
} Variant397EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant397EntryQuad;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant397EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant397EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_E88[0x24];
    MATRIX matrix, matrix0, matrix1;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant397EntryConfig *G32 config;
    u32 field_F58;
    GsCOORDUNIT *G32 parts[3];
    s16 field_F68, field_F6A, field_F6C, field_F6E;
    s32 field_F70, field_F74, phase;
    u8 unknown_F7C[8];
    s32 tint;
    s16 slot, command;
} Variant397EntryState;

extern u8 D_8013DDE4[];
void func_8013C22C(u8 *context);
void func_8013C994(u8 *context);
void func_8013CE7C(u8 *context);
#endif
