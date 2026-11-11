#ifndef FRENCH336_RIBBON_VIEW_H
#define FRENCH336_RIBBON_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantStripSpine spine;
    u8 color[4];
    u8 unknown224[0xA4];
    VECTOR origin;
    VECTOR end;
    s32 state;
    s32 count;
    s32 extent;
    long flag[17];
    ModelVariantStripOffsets offsets;
} ModelVariant336Ribbon;

#endif
