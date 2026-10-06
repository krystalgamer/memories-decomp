#ifndef SPANISH_MODEL_VARIANT417_LINES_H
#define SPANISH_MODEL_VARIANT417_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[2][4][6];
    u8 color[4];
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines417;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Companion417;

typedef struct {
    u8 unknown_00[0x54C];
    Lines417 groups[3];
    u8 unknown_A2C[0x6C];
    Companion417 companion;
    u8 unknown_B24[0x1598];
    GsGLINE line;
    u8 unknown_20D0[0x14];
    s32 origin[3];
    SVECTOR target;
    u8 unknown_20F8[4];
    s32 direction_a[3];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_2118[0x18];
    u32 step;
    u8 unknown_2134[0x3C];
    s32 phase;
} Lines417State;

#endif
