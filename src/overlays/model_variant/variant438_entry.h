#ifndef MEMORIES_MODEL_VARIANT438_ENTRY_H
#define MEMORIES_MODEL_VARIANT438_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x44-byte row of header 438's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 count;
    s32 mode;
    u8 unknown_24[4];
    u32 start;
    u8 unknown_2C[0xC];
    u32 track_end, sweep_start, sweep_end;
} Variant438EntryConfig;

/* Up to sixty-four swarm quads at the start of the work area; the swarm
 * helper animates them from the part matrices captured at launch. */
typedef struct {
    SVECTOR a[64], b[64], c[64], d[64], origin[64];
    u8 color[4];
    u8 unknown_A04[0x10];
    s32 size[64];
    s32 field_B14[64];
    s32 field_C14[64];
    s32 done[64];
    MATRIX origins[64];
} Variant438EntrySwarm;

typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 offset[9];
    s32 field_A0, field_A4;
} Variant438EntryRecord;

typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2], cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant438EntryBand;

typedef struct {
    SVECTOR point[11], corner[4];
    u8 inner[4], outer[4];
    s32 size, field_84, done;
    u8 unknown_8C[4];
} Variant438EntryQuad;

typedef struct {
    Variant438EntrySwarm swarm;
    Variant438EntryRecord records[16];
    ModelVariantWebNarrow webs[3];
    Variant438EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant438EntryQuad quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite;
    u8 unknown_2EC8[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[64];
    VECTOR offsets[64];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant438EntryConfig *G32 config;
    u32 field_3758;
    GsCOORDUNIT *G32 parts[3];
    s16 field_3768, field_376A;
    s32 sweep;
    s16 field_3770, field_3772;
    s32 field_3774, field_3778, phase;
    u8 unknown_3780[8];
    s32 tint;
    s16 slot, command;
} Variant438EntryState;

extern u8 gModelVariant438_D1988[];
void FrenchModelVariant455_DrawSwarm(u8 *context);
#endif
