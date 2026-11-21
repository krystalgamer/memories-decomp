#ifndef VARIANT448_ORBIT_H
#define VARIANT448_ORBIT_H
#include "model_variant.h"
typedef struct {
    SVECTOR points[4][4];
    CVECTOR outer;
    CVECTOR inner;
    /* The sheet records' size word (ModelVariantSheet.size): the entry
     * zeroes it, the spoke helper scales each ring by sheet_size * angle /
     * 1024 and the web helper tests it against 0x800. */
    s32 sheet_size, field_8C, field_90;
    u8 unknown_94[4];
    s32 size[3][2][3];
    s32 done[3][2][3];
} Variant448Orbit;
#endif
