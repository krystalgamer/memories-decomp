#ifndef FRENCH_MODEL_VARIANT427_STREAMERS_H
#define FRENCH_MODEL_VARIANT427_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* The MODEL458 streamer with 0x44 further bytes before the depths, giving a
 * 0x378-byte stride. */
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
    u8 unknown_2AC[0x44];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Streamer427;

#endif
