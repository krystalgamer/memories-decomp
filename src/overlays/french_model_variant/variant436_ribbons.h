#ifndef FRENCH_MODEL_VARIANT436_RIBBONS_H
#define FRENCH_MODEL_VARIANT436_RIBBONS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbons436Screen;

typedef struct {
    SVECTOR a[9];
    Ribbons436Screen sa[9];
    s32 angle[9];
    SVECTOR b[9];
    Ribbons436Screen sb[9];
    s32 width[9];
    u8 color[4];
    u8 unknown124[0x84];
    s32 otz[9];
    s16 ox[9];
    s16 oy[9];
    s32 start;
    s32 end;
    s32 state;
    s32 count;
} Ribbons436Ribbon;

typedef struct {
    u8 unknown00[0x24];
    s32 wave_end;
    s32 loop_end;
} Ribbons436Timing;

#endif
