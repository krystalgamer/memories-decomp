#ifndef FRENCH_MODEL_VARIANT417_FEEDBACK_H
#define FRENCH_MODEL_VARIANT417_FEEDBACK_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[3][17];
    CVECTOR inner, outer;
    s32 progress;
    s32 cycles;
} Feedback417;

typedef struct {
    u8 unknown00[0x120];
    s32 size;
} Feedback417SizeRecord;

typedef struct {
    u8 unknown00[0xF28];
    Feedback417 groups[5];
    u8 unknown1770[0x2028 - 0x1770];
    POLY_GT4 quad;
    u8 unknown205C[0x20F0 - 0x205C];
    SVECTOR origin;
    VECTOR direction;
    u8 unknown2108[0x2130 - 0x2108];
    s32 step;
    u8 unknown2134[0x2170 - 0x2134];
    s32 phase;
} Feedback417State;

#endif
