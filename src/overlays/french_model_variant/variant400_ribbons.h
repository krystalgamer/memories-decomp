#ifndef FRENCH_MODEL_VARIANT400_RIBBONS_H
#define FRENCH_MODEL_VARIANT400_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Model400Screen;

typedef struct {
    SVECTOR a[17];
    Model400Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Model400Screen sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown224[0xA4];
    VECTOR position;
    VECTOR delta;
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Model400FirstRibbon;

#endif
