#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT476_SCREEN_GRID_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT476_SCREEN_GRID_H

#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[9][9];
    SVECTOR b[9][9];
    u8 color[9][4];
} Variant476ScreenGrid;

#endif
