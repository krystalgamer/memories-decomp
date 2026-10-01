#ifndef FRENCH465_FAN_VIEW_H
#define FRENCH465_FAN_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[18];
    SVECTOR outer[17];
    u8 center_color[4];
    u8 middle_color[4];
    u8 outer_color[4];
    s32 size;
} Variant465Fan;

#endif
