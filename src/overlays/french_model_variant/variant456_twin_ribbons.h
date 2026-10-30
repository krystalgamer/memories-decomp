#ifndef FRENCH_MODEL_VARIANT456_TWIN_RIBBONS_H
#define FRENCH_MODEL_VARIANT456_TWIN_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} TwinRibbon456Screen;

typedef struct {
    SVECTOR a[3];
    TwinRibbon456Screen sa[3];
    s32 angle[3];
    SVECTOR b[3];
    TwinRibbon456Screen sb[3];
    s32 width[3];
    u8 color[4];
    u8 unknown64[0x1C];
    s32 scale;
    u8 unknown84[0x24];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    s32 amp[3];
    s32 state;
    u8 unknownD0[0x10];
    s32 dir[3];
    u8 unknownEC[4];
} TwinRibbon456;

#endif
