#ifndef FRENCH336_RIBBON_VIEW_H
#define FRENCH336_RIBBON_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown224[0xA4];
    VECTOR origin;
    VECTOR end;
    s32 state;
    s32 count;
    s32 extent;
    PSXLONG flag[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} ModelVariant336Ribbon;

#endif
