#ifndef FRENCH_MODEL_VARIANT426_RIBBON_H
#define FRENCH_MODEL_VARIANT426_RIBBON_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[17];
    ModelVariantScreenPoint sa[17];
    s32 angle[17];
    SVECTOR b[17];
    ModelVariantScreenPoint sb[17];
    s32 width[17];
    ModelVariantStripStyle style;
    u8 unknown_240[0x88];
    VECTOR position;
    ModelVariantStripOffsets offsets;
} Ribbon426;

#endif
