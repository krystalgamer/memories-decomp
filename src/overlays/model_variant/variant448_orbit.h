#ifndef VARIANT448_ORBIT_H
#define VARIANT448_ORBIT_H
#include "model_variant.h"
typedef struct {
    SVECTOR points[4][4];
    CVECTOR outer;
    CVECTOR inner;
    s32 field_88, field_8C, field_90;
    u8 unknown_94[4];
    s32 size[3][2][3];
    s32 done[3][2][3];
} Variant448Orbit;
#endif
