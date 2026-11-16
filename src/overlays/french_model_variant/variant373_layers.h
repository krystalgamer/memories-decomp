#ifndef FRENCH_MODEL_VARIANT373_LAYERS_H
#define FRENCH_MODEL_VARIANT373_LAYERS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[51];
    SVECTOR rotation;
    s32 scale, field_1A4, done;
} Family373Layer;

#endif
