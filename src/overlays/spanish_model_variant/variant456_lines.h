#ifndef SPANISH_MODEL_VARIANT456_LINES_H
#define SPANISH_MODEL_VARIANT456_LINES_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[6];
    CVECTOR color;
    u8 unknown_34[0x1C];
} Lines456;

typedef struct {
    s32 progress;
    u8 unknown_04[0xC];
    s32 origin_x;
    s32 origin_y;
    s32 origin_z;
    u8 unknown_1C[0xD4];
} Lines456Companion;

typedef struct {
    s32 size;
    u8 unknown_04[0x2C];
    s32 fading;
    s32 intensity;
    u8 unknown_38[0x88];
} Lines456Control;

typedef struct {
    Lines456 groups[2];
    u8 unknown_A0[0xC0];
    Lines456Companion companions[2];
    u8 unknown_340[0x12C8];
    Lines456Control controls[2];
    u8 unknown_1788[0xB2C];
    GsGLINE line;
    u8 unknown_22C8[0x2C];
    s32 direction_a[2];
    u8 unknown_22FC[4];
    DVECTOR projected;
    s32 direction[3];
    u8 unknown_2310[0xC];
    u32 frame;
    u8 unknown_2320[8];
    s32 step;
    u8 unknown_232C[0x3C];
    s32 angle;
} Lines456State;

#endif
