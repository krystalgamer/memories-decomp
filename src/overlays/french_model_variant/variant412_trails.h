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

typedef struct {
    u8 before_capture[20];
    u32 capture_start;
    u32 capture_stop;
} Variant412Timing;

typedef struct {
    Variant412Timing *G32 timing;
} Variant412TimingLink;

typedef struct {
    GsCOORDINATE2 *G32 part[6];
} Variant412Links;

#endif
