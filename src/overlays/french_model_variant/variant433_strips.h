#ifndef FRENCH433_STRIPS_VIEW_H
#define FRENCH433_STRIPS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

struct ModelVariant433Strip {
    SVECTOR a[2];
    SVECTOR b[2];
    SVECTOR c[2];
    PSXLONG sa[2];
    PSXLONG sb[2];
    PSXLONG sc[2];
    u8 unknown48[0x20];
    s32 progress[2];
    s32 done[2];
    u8 ca[2][4];
    u8 cb[2][4];
    u8 unknown88[0x18];
    s32 otz[2];
};

#endif
