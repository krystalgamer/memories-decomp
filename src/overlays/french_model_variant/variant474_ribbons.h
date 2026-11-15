#ifndef FRENCH474_RIBBONS_H
#define FRENCH474_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    long packed;
} Ribbon474Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon474Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon474Screen sb[17];
    s32 width[17];
    s32 phase[17];
    s32 amp[17];
    CVECTOR color[17];
    u8 unknown2EC[0xF4];
    long flags[17];
    ModelVariantStripOffsets offsets;
} Ribbon474;

#endif
