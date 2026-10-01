#ifndef FRENCH402_RIBBONS_VIEW_H
#define FRENCH402_RIBBONS_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
    u8 unknown_50[8];
} Family402Ribbon;

void func_8013C048(u8 *context);
#endif
