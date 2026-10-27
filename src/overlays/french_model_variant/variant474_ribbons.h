#ifndef FRENCH474_RIBBONS_H
#define FRENCH474_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon474Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon474Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon474Screen sb[17];
    s32 width[17];
    s32 phase[17];
    s32 amp[17];
    CVECTOR color[17];
    u8 unknown2EC[0xF4];
    PSXLONG flags[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Ribbon474;

typedef struct {
    u8 unknown00[0x18];
    u32 grow_start;
    u32 grow_end;
    u32 spin_end;
} Ribbon474Timing;

#endif
