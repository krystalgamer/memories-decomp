#ifndef FRENCH_MODEL_VARIANT388_PETALS_H
#define FRENCH_MODEL_VARIANT388_PETALS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x84-byte petal arm of this family: the MODEL418 spiral arm moved
 * 16 bytes in, with the colours before the projection flags. */
typedef struct {
    u8 unknown_00[0x10];
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 cb[2][4];
    u8 ca[2][4];
    u8 unknown_60[4];
    PSXLONG flag[2];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown_7C[8];
} Petal388Arm;

#endif
