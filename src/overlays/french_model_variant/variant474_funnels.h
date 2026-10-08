#ifndef FRENCH474_FUNNELS_VIEW_H
#define FRENCH474_FUNNELS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} Funnel474;

typedef struct {
    u8 unknown_00[0x264];
    s32 size;
} Funnel474TriggerView;

typedef struct {
    Funnel474 funnels[2];
    Funnel474TriggerView trigger;
    u8 unknown_0498[0x14CC];
    POLY_GT4 quad;
    u8 unknown_1998[0x1C4];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_1B70[0x20];
    s32 frame;
    u8 unknown_1B94[8];
    s32 step;
    u8 unknown_1BA0[0x60];
    CVECTOR inner_color;
    CVECTOR outer_color;
    s32 spin;
    u8 unknown_1C0C[0x28];
    s16 side;
} Funnels474State;

#endif
