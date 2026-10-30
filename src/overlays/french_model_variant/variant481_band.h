#ifndef FRENCH_MODEL_VARIANT481_BAND_H
#define FRENCH_MODEL_VARIANT481_BAND_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* The entry's 0x8EC-byte band of 33 steps (Variant464EntryBand), viewed by
 * its renderer: a spine and an offset edge per step, their packed screen
 * words, and the per-step half-width vectors of the quads. offset[] starts
 * staggered at -128 * step and grows to 0x800. */
typedef struct {
    SVECTOR spine[33];
    PSXLONG screen[33];
    s32 heading[33];
    SVECTOR edge[33];
    PSXLONG edge_screen[33];
    s32 length[33];
    s32 amount[33];
    s32 offset[33];
    CVECTOR color[33];
    u8 unknown_5AC[0x1B4];
    PSXLONG flag[33];
    s32 otz[33];
    s16 dx[33];
    s16 dy[33];
} Band481;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    u8 unknown_000[0x230];
    Band481 bands[1];
    u8 unknown_B1C[0x1350];
    POLY_GT4 beams[2];
    u8 unknown_1ED4[0x108];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    u32 elapsed;
    u32 animation;
    s32 step;
    u8 unknown_2040[0x7C];
    s32 phase;
    u8 unknown_20C0[0x14];
    s16 slot;
} Band481State;

void func_8013BE38(u8 *context);

#endif
