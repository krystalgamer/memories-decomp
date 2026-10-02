#ifndef MEMORIES_DECOMP_MODEL_VARIANT459_GRID_H
#define MEMORIES_DECOMP_MODEL_VARIANT459_GRID_H

#include "model_variant.h"

/* The header-459 grid: nine rows of seventeen points and a colour per row. */
typedef struct {
    SVECTOR point[9][17];
    u8 color[9][4];
} Variant459Grid;

#endif
