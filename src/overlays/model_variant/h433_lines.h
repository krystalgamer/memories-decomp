#ifndef SPANISH_MODEL_VARIANT450_LINES_H
#define SPANISH_MODEL_VARIANT450_LINES_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

/* Views the 450 quad and line helpers keep over the 433 entry state: each
 * primary starts at a ribbon's phase words (+0x1D0) and each fade record at
 * the next sheet's size (+0x88). */
typedef struct {
    s32 threshold;
    u8 unknown_04[36];
    VECTOR translation;
    u8 unknown_38[480];
} Variant450Primary;

typedef struct {
    s32 scale;
    u8 unknown_04[12];
    s32 fading;
    s32 brightness;
    u8 unknown_18[136];
} Variant450Fade;

#endif
