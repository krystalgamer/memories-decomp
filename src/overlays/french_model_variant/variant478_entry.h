#ifndef FRENCH_MODEL_VARIANT478_ENTRY_H
#define FRENCH_MODEL_VARIANT478_ENTRY_H

#include "../../types.h"
#include "../french_model_variant/variant402_entry.h"
#include "../french_model_variant/variant478_swarm.h"
#include "../spanish_model_variant/variant478_quads.h"
#include "../spanish_model_variant/variant478_rings.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"

typedef struct {
    CVECTOR inner, outer, grid;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, swarm_start, swarm_end, ring_end, fade_start;
} EntryConfig478;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag, target;
} EntryProjection478;

typedef struct {
    SVECTOR a[32], b[32], c[32], d[32], positions[32];
    u8 color[4], unknown_504[0x10];
    s32 size[32], done[32], reset[32];
} EntrySwarm478;

typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    u8 color[4];
    s32 size[10], angle[10], done[10], field_20C[10], reset[10];
    u8 unknown_25C[0xA0];
} EntryQuads478;

typedef struct {
    SVECTOR v0[8], v1[8], v2[8], v3[8], positions[8];
    u8 color[4];
    s32 size[8], field_164[8], hidden[8], field_1A4[8], field_1C4[8];
    u8 unknown_1E4[0x80];
} EntryPetals478;

typedef struct {
    u8 unknown_00[0x48];
    u8 color[9][4];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 progress[9], field_A0, field_A4;
} EntryExtra478;

typedef struct {
    u8 unknown_00[0x68];
    s32 progress[2];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} EntryPair478;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, field_8C;
} EntryRing478;

typedef struct {
    SVECTOR center, first[5], second[5], corners[4];
    CVECTOR inner, outer;
    s32 field_80, field_84, field_88, field_8C;
} EntrySmall478;

typedef struct {
    EntryQuads478 quads[1];
    EntryPetals478 petals[3];
    EntrySwarm478 swarm;
    EntryExtra478 extras[16];
    ModelVariantWebNarrow grids[3];
    EntryPair478 pairs[1];
    ModelVariantSheet sheets[2];
    EntryRing478 rings[6], large_rings[4];
    EntrySmall478 small[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 unknown_28C4[0x34];
    POLY_FT4 flat[2], swarm_packet;
    u8 unknown_2970[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig478 *G32 config;
    s32 field_2A00;
    GsCOORDUNIT *G32 parts[3];
    s16 field_2A10, field_2A12, field_2A14, field_2A16;
    s32 field_2A18, wave, field_2A20, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState478;

extern u8 D_8013D220[];
void func_8013C34C(u8 *context);
void func_8013C898(u8 *context);
void func_8013CDA0(u8 *context);

#endif
