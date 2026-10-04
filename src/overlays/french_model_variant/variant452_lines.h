#ifndef FRENCH_MODEL_VARIANT452_LINES_H
#define FRENCH_MODEL_VARIANT452_LINES_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR inner[6][6];
    SVECTOR outer[6][6];
    u8 color[3];
    u8 unknown243[0x11];
    s32 count;
    u8 unknown258[8];
} Variant452Grid;

/* Minimum observed view, not an allocation-capacity declaration. */
typedef struct {
    Variant452Grid grid[3];
    u8 unknown720[0x2BAC];
    GsGLINE line;
    u8 unknown32E0[0x28];
    s32 translation[3];
    u8 unknown3314[8];
    s32 delta_x;
    s32 delta_y;
    u8 unknown3324[0x14];
    DVECTOR screen_delta;
    VECTOR delta;
    u8 unknown334C[0x14];
    s32 step;
    u8 unknown3364[0x44];
    s32 phase;
} Variant452LinesView;

#endif
