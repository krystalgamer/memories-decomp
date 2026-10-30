#ifndef MEMORIES_MODEL_VARIANT430_ENTRY_H
#define MEMORIES_MODEL_VARIANT430_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x34-byte row of header 430's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, end;
    u8 unknown_24[4];
    u32 track_end;
    u8 unknown_2C[8];
} Variant430EntryConfig;

/* Thirty-two swarm quads at the start of the work area, drawn by
 * func_8013C0FC. */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32], origin[32];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32];
    s32 field_594[32];
    s32 field_614[32];
} Variant430EntrySwarm;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant430EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant430EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant430EntryQuad;

typedef struct {
    Variant430EntrySwarm swarm;
    Variant430EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant430EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant430EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_1F48[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant430EntryConfig *G32 config;
    u32 field_1FD8;
    GsCOORDUNIT *G32 parts[3];
    s16 field_1FE8, field_1FEA;
    s16 field_1FEC, field_1FEE;
    s32 field_1FF0, field_1FF4, phase;
    u8 unknown_1FFC[8];
    s32 tint;
    s16 slot, command;
} Variant430EntryState;

/* Nine texture images followed by the command table. */
extern GsIMAGE D_8013C7B8[];
extern Variant430EntryConfig D_8013C8B4[];
void func_8013C0FC(u8 *context);
#endif
