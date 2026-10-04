#ifndef FRENCH335_RIBBON_VIEW_H
#define FRENCH335_RIBBON_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Model335Screen;

typedef struct {
    SVECTOR a[17];
    Model335Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Model335Screen sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown224[0xA4];
    s32 otz[17];
    PSXLONG flag[17];
    s16 ox[17];
    s16 oy[17];
} Model335Ribbon;
#endif
