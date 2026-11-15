#ifndef SPANISH_MODEL_VARIANT456_SPRAYS_H
#define SPANISH_MODEL_VARIANT456_SPRAYS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x108-byte spray of header 456: a colour, the growth of each of its
 * seventeen points, the spin added to its direction and a hide flag. */
typedef struct {
    CVECTOR color;
    u8 unknown_04[0xB4];
    s32 growth[17];
    s32 spin;
    s32 hidden;
    u8 unknown_104[4];
} Spray456;

#endif
