#ifndef FRENCH_MODEL_VARIANT412_TRAILS_H
#define FRENCH_MODEL_VARIANT412_TRAILS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR original[6][31];
    SVECTOR moved[6][31];
    s32 angle[31];
    s32 progress[31];
    CVECTOR inner[6][31];
    CVECTOR outer[6][31];
} Variant412Trail;

#endif
