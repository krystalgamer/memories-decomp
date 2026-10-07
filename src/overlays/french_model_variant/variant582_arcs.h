#ifndef FRENCH582_ARCS_VIEW_H
#define FRENCH582_ARCS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR near_points[2][5];
    SVECTOR far_points[2][5];
    u8 unknown_A0[0x14];
    s32 near_progress[2];
    s32 far_progress[2];
    s32 done[2];
    u8 unknown_CC[4];
} Arcs582Group;

typedef struct {
    u8 unknown_00[0x28];
    u32 limit;
} Arcs582Timing;

typedef struct {
    Arcs582Group groups[4];
    u8 unknown_340[0x316C];
    GsGLINE line;
    u8 unknown_34C0[0x20];
    s32 origin[3];
    u8 unknown_34EC[0x160];
    s32 direction_a[2];
    u8 unknown_3654[4];
    s32 direction[3];
    u8 unknown_3664[4];
    DVECTOR projected;
    u8 unknown_366C[0x300];
    s32 axis[3];
    u8 unknown_3978[0x10];
    u32 time;
    u8 unknown_398C[4];
    s32 step;
    u8 unknown_3994[4];
    Arcs582Timing *G32 timing;
    u8 unknown_399C[0x3C];
    s32 phase;
} Arcs582State;

#endif
