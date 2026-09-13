#ifndef MEMORIES_DECOMP_MODEL_RING_EFFECT_H
#define MEMORIES_DECOMP_MODEL_RING_EFFECT_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "func_80057E20.h"

#define MODEL_RING_VERTEX_COUNT 67

/* Twelve-byte view of the six halfwords owned by model_geometry_tables.c.
 * The final halfword has no established role. */
typedef struct {
    CVECTOR color;
    s16 minimum_radius;
    s16 fade_divisor;
    s16 growth_divisor;
    s16 pad_0A;
} RingSettings;

/* Only the consumed prefix of borrowed module-data storage is described.
 * Callers must provide this complete aligned prefix, initialize with a
 * nonnegative command, then retain it for subsequent negative updates. */
typedef struct {
    RingSettings *settings;
    SVECTOR vertices[MODEL_RING_VERTEX_COUNT];
    CVECTOR color;
    u32 field_220;
    s32 fade;
    s32 growth;
    u8 phase;
    u8 pad_22D[3];
} RingWork;

/* Reconstructed local frame interval C0..DF, followed by screen[] at E0.
 * Only value is passed to func_80057E20, whose output is eight bytes.
 * The original purpose of the remaining 24 bytes is unknown; frame_bytes
 * records stack reservation, not a 32-byte callee output or four records. */
typedef union {
    ModelEffectAdjustment value;
    u8 frame_bytes[0x20];
} RingQueryFrameReservation;

typedef char RingSettings_size_must_be_12[
    sizeof(RingSettings) == 12 ? 1 : -1
];
typedef char RingSettings_offsets_must_match[
    (u32)&((RingSettings *)0)->minimum_radius == 4 &&
    (u32)&((RingSettings *)0)->fade_divisor == 6 &&
    (u32)&((RingSettings *)0)->growth_divisor == 8 ? 1 : -1
];
typedef char RingWork_prefix_size_must_be_0x230[
    sizeof(RingWork) == 0x230 ? 1 : -1
];
typedef char RingWork_offsets_must_match[
    (u32)&((RingWork *)0)->vertices == 4 &&
    (u32)&((RingWork *)0)->color == 0x21C &&
    (u32)&((RingWork *)0)->field_220 == 0x220 &&
    (u32)&((RingWork *)0)->fade == 0x224 &&
    (u32)&((RingWork *)0)->growth == 0x228 &&
    (u32)&((RingWork *)0)->phase == 0x22C ? 1 : -1
];
typedef char RingQueryFrameReservation_sizes_must_match[
    sizeof(ModelEffectAdjustment) == 8 &&
    sizeof(RingQueryFrameReservation) == 0x20 ? 1 : -1
];

s32 func_8006C37C(RingWork *state, s32 command);

#endif
