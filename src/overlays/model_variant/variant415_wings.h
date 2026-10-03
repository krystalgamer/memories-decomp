#ifndef MEMORIES_MODEL_VARIANT415_WINGS_H
#define MEMORIES_MODEL_VARIANT415_WINGS_H

#include "model_variant.h"

/* Three 0x74-byte bands at the start of the header-415 work area.
 * Each has three two-point rows, their packed screen coordinates,
 * and a growth level and depth per column. */
typedef struct {
    SVECTOR a[2], b[2], c[2];
    PSXLONG sa[2], sb[2], sc[2];
    u8 unknown48[0x1C];
    s32 level[2], otz[2];
} Variant415Wing;

typedef char Variant415Wing_size_must_be_0x74[sizeof(Variant415Wing) == 0x74 ? 1 : -1];

#endif
