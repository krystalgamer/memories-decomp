#ifndef VARIANT448_ORBIT_H
#define VARIANT448_ORBIT_H
#include "model_variant.h"
typedef struct {
    SVECTOR points[4][4];
    CVECTOR outer;
    CVECTOR inner;
    u8 unknown[16];
    s32 size[3][2][3];
    s32 done[3][2][3];
} Variant448Orbit;
#endif
