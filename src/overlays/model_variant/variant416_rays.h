#ifndef VARIANT416_RAYS_H
#define VARIANT416_RAYS_H
#include "model_variant.h"
typedef struct {
    SVECTOR point[9];
    CVECTOR color[9];
    u8 unknown[16];
    s32 radius[9];
    u8 tail[8];
} Variant416Ray;
#endif
