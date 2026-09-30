#ifndef FRENCH402_BANDS_VIEW_H
#define FRENCH402_BANDS_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 scale;
    s32 cycles;
} Family402Band;

void func_8013CB38(u8 *context);
#endif
