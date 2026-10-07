#ifndef FRENCH_MODEL_VARIANT464_RINGS_H
#define FRENCH_MODEL_VARIANT464_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x1E4-byte ring: three rows of seventeen points, the growth
 * progress and the completion state left by the last wrap. */
typedef struct {
    SVECTOR points[3][17];
    u8 unknown_198[0x44];
    s32 progress;
    s32 state;
} Ring464;

typedef struct {
    u8 unknown_00[0x114C];
    Ring464 rings[3];
    u8 unknown_16F8[0x3F0];
    POLY_GT4 quad;
    u8 unknown_1B1C[0xBC];
    s32 origin[3];
    u8 unknown_1BE4[8];
    s32 direction[3];
    u8 unknown_1BF8[0x2C];
    s32 step;
    u8 unknown_1C28[0x38];
    s32 phase;
} Ring464State;

#endif
