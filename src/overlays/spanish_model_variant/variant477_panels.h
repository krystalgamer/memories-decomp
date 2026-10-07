#ifndef SPANISH_MODEL_VARIANT477_PANELS_H
#define SPANISH_MODEL_VARIANT477_PANELS_H

#include "../../types.h"
#include "variant477_quads.h"

/* One of the two 0x8C-byte panels at the start of the MODEL477 context: a
 * single textured quad with its colour, size, completion flag, base matrix
 * and growth direction. */
typedef struct {
    SVECTOR a[1];
    SVECTOR b[1];
    SVECTOR c[1];
    SVECTOR d[1];
    u8 unknown_20[8];
    u8 color[4];
    u8 unknown_2C[0x10];
    s32 size[1];
    s32 done[1];
    u8 unknown_44[8];
    MATRIX matrix[1];
    u8 unknown_6C[0x10];
    VECTOR direction[1];
} Panel477;

#endif
