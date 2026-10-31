#ifndef MODEL_VARIANT426_LINES_H
#define MODEL_VARIANT426_LINES_H

#include "../../types.h"
#include "model_variant.h"

/* The header-459 line groups (spanish_model_variant/variant459_lines.h) at the
 * North American header-426 work offsets. */
typedef struct {
    SVECTOR points[2][4][6];
    u8 color[4];
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} Lines426;

typedef struct {
    u8 unknown_00[0x38];
    u32 cycle_start;
    u32 cycle_end;
} Lines426Timing;

typedef struct {
    u8 unknown_0000[0x1928];
    Lines426 groups[3];
    u8 unknown_1E08[0x1714];
    GsGLINE line;
    u8 unknown_3530[0x70];
    s32 origin[3];
    u8 unknown_35AC[4];
    SVECTOR target;
    u8 unknown_35B8[4];
    s32 direction_a[3];
    u8 unknown_35C8[0x30];
    DVECTOR projected;
    u8 unknown_35FC[0xC];
    s32 direction[3];
    u8 unknown_3614[0x10];
    u32 time;
    u8 unknown_3628[4];
    u32 step;
    u8 unknown_3630[4];
    Lines426Timing *G32 timing;
    u8 unknown_3638[0x60];
    s32 phase;
} Lines426State;

#endif
