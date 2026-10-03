#ifndef MEMORIES_MODEL_VARIANT398_SCREEN_RINGS_H
#define MEMORIES_MODEL_VARIANT398_SCREEN_RINGS_H

#include "model_variant.h"

/* Observed 0x1E4-byte record stride in the USA header-398 renderer. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    u8 unknown198[0x44];
    s32 scale, count;
} Variant398ScreenRing;

typedef char Variant398ScreenRing_size_must_be_0x1E4[sizeof(Variant398ScreenRing) == 0x1E4 ? 1 : -1];

#endif
