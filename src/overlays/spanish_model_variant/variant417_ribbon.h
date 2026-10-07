#ifndef SPANISH_MODEL_VARIANT417_RIBBON_H
#define SPANISH_MODEL_VARIANT417_RIBBON_H

#include "../../types.h"
#include "variant441_ribbon.h"

/* MODEL417 keeps the MODEL441 ribbon state 0x5AC bytes lower. */
typedef struct {
    u8 unknown_00[0xA2C];
    Ribbon441 ribbons[1];
    u8 unknown_0A98[0x1528];
    POLY_GT4 quad;
    u8 unknown_1FF4[0xF0];
    s32 origin[3];
    u8 unknown_20F0[8];
    s32 direction[3];
    u8 unknown_2104[4];
    DVECTOR projected;
    u8 unknown_210C[0x1C];
    s32 time;
    u8 unknown_212C[0xC];
    Ribbon441Timing *G32 timing;
    u8 unknown_213C[0x10];
    s32 index;
    u8 unknown_2150[0x18];
    s16 progress;
    s16 width;
    u8 unknown_216C[4];
    s32 phase;
} Ribbon417State;

#endif
