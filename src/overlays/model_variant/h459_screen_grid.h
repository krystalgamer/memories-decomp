#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT476_SCREEN_GRID_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT476_SCREEN_GRID_H

#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[9][9];
    SVECTOR b[9][9];
    CVECTOR color[9];
} Variant476ScreenGrid;

#endif
