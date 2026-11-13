#ifndef FRENCH_MODEL_VARIANT427_STREAMERS_H
#define FRENCH_MODEL_VARIANT427_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* The MODEL458 streamer with 0x44 further bytes before the depths, giving a
 * 0x378-byte stride. */
typedef struct {
    ModelVariantStripSpine spine;
    u8 unknown220[0x44];
    u8 color[17][4];
    s32 field_2A8;
    u8 unknown_2AC[0x44];
    ModelVariantStripOffsets offsets;
} Streamer427;

#endif
