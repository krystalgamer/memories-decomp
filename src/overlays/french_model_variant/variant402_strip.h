#ifndef FRENCH402_STRIP_VIEW_H
#define FRENCH402_STRIP_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant402_rings.h"

typedef struct {
    SVECTOR points[3][2];
    PSXLONG projected[3][2];
    u8 unknown_48[8];
    s32 depth[2];
} Family402Strip;

void func_8013BA5C(u8 *context);
#endif
