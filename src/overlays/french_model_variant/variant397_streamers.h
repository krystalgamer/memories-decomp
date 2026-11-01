#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT397_STREAMERS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT397_STREAMERS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x378-byte streamer: a seventeen-point spine and its copy moved along
 * the view, both projected, with per-point angle, width, colour, projection
 * flag, depth and screen offset. */
typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 unknown220[0x44];
    u8 color[17][4];
    u8 unknown2A8[0x04];
    PSXLONG flag[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Variant397Streamer;

/* Command row view: only the record count is read. */
typedef struct {
    u8 unknown_00[0x24];
    s32 count;
} Variant397StreamerTiming;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    u8 unknown_0000[0x1A70];
    VECTOR positions[5];
    u8 unknown_1AC0[0x1B24 - 0x1AC0];
    s32 view_x, unknown_1B28, view_z;
    u8 unknown_1B30[0x1B3C - 0x1B30];
    u32 frame;
    u8 unknown_1B40[8];
    s32 step;
    u8 unknown_1B4C[8];
    Variant397StreamerTiming *G32 timing;
    u8 unknown_1B58[0x1BA0 - 0x1B58];
    s32 length[5];
    s32 rx[5];
    s32 ry[5];
    s32 rz[5];
    u8 unknown_1BF0[8];
    s32 phase;
} Variant397StreamerState;

void FrenchModelVariant397_DrawStreamers(u8 *ctx);
#endif
