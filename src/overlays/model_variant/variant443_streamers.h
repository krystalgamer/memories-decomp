#ifndef MEMORIES_DECOMP_MODEL_VARIANT443_STREAMERS_H
#define MEMORIES_DECOMP_MODEL_VARIANT443_STREAMERS_H

#include "model_variant.h"

/* One 0x378-byte streamer of header 443: a seventeen-point spine and a copy
 * of it moved along the view, both projected, and per point the screen angle,
 * the projected width, a colour, the depth and the width's screen offset. */
typedef struct {
    ModelVariantStripSpine spine;
    u8 unknown220[0x44];
    u8 color[17][4];
    u8 unknown2A8[0x48];
    ModelVariantStripOffsets offsets;
} Variant443Streamer;

#endif
