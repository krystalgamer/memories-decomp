#ifndef MEMORIES_DECOMP_SOUND_EFFECT_REQUEST_H
#define MEMORIES_DECOMP_SOUND_EFFECT_REQUEST_H

#include "../types.h"

/* Local input snapshots, not a sound-driver memory record. */
typedef union {
    u64 all;
    struct { s32 original; s32 copy; } words;
} SDSEPlayIdPair;

#endif
