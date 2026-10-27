#ifndef MEMORIES_MODEL_VARIANT402_ENTRY_H
#define MEMORIES_MODEL_VARIANT402_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant439_entry.h"

/* One 0x34-byte row of header 402's command table. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[0x0D];
    u32 start;
    u8 unknown_24[4];
    u32 follow_limit;
    u8 unknown_2C[8];
} Variant402EntryConfig;

/* Ten growing quads launched along the target direction; the quads helper
 * animates them. */
typedef struct {
    SVECTOR a[10], b[10], c[10], d[10];
    SVECTOR position[10];
    u8 color[4];
    s32 size[10], angle[10], done[10];
    u8 unknown_20C[0x28];
    s32 reset[10];
    u8 unknown_25C[0xA0];
} Variant402EntryQuads;

/* The swarm helper's thirty-two quads, laid out like the quad group. */
typedef struct {
    SVECTOR a[32], b[32], c[32], d[32];
    SVECTOR position[32];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32], done[32], reset[32];
} Variant402EntrySwarm;

/* One 0xA8-byte streak: nine fading colours and depths. */
typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74;
    u8 unknown_78[4];
    s32 depth[9];
    s32 field_A0, field_A4;
} Variant402EntryStreak;

/* One 0xA0-byte band with two depth rows. */
typedef struct {
    u8 unknown_00[0x68];
    s32 depth[2];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} Variant402EntryBand;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant402EntryQuads quads[1];
    Variant402EntrySwarm swarm;
    Variant402EntryStreak streaks[16];
    ModelVariantWebNarrow webs[3];
    Variant402EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantSpokeRing spokes[4];
    Variant439EntryFan fans[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], glow;
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

extern u8 D_8013CD0C[];
extern u8 D_8013CD44[];
void func_8013C1E0(u8 *context);
void func_8013C778(u8 *context);
#endif
