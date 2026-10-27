#ifndef MEMORIES_MODEL_VARIANT402_ENTRY_H
#define MEMORIES_MODEL_VARIANT402_ENTRY_H
#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x34-byte command row: three colours, the three part indices, the
 * swarm spin, the swarm start frame, the emission end and the tracking end. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u16 spin, unknown_1E;
    u32 start, limit, track_end;
    u8 unknown_2C[8];
} Variant402EntryConfig;

/* Ten growing quads (drawn by func_8013C778). */
typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], origin[10];
    CVECTOR color;
    s32 size[10];
    s32 angle[10];
    s32 done[10];
    s32 field_20C[10];
    s32 reset[10];
    u8 unknown_25C[0xA0];
} Variant402EntryQuads;

/* Thirty-two swarm quads (drawn by func_8013C1E0). */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32], origin[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 size[32];
    s32 done[32];
    s32 field_614[32];
} Variant402EntrySwarm;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant402EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant402EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant402EntryQuad;

typedef struct {
    Variant402EntryQuads quads[1];
    Variant402EntrySwarm swarm;
    Variant402EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant402EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant402EntryQuad quad_records[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_2244[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant402EntryConfig *G32 config;
    u32 field_22D4;
    GsCOORDUNIT *G32 parts[3];
    s16 field_22E4, field_22E6, field_22E8, field_22EA;
    s32 field_22EC, field_22F0, phase;
    u8 unknown_22F8[8];
    s32 tint;
    s16 slot, command;
} Variant402EntryState;

extern u8 D_8013CC48[];
void func_8013C1E0(u8 *context);
void func_8013C778(u8 *context);
#endif
