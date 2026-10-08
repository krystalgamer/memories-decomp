#ifndef FRENCH_MODEL_VARIANT443_BANDS_H
#define FRENCH_MODEL_VARIANT443_BANDS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[2], b[2], c[2];
    PSXLONG sa[2], sb[2], sc[2];
    u8 ca[2][4], cb[2][4];
    u8 unknown58[0x18];
    s32 otz[2];
} Band443;

typedef struct {
    u8 unknown00[0x3C];
    u32 grow_start, grow_end;
} Bands443Timing;

typedef struct {
    u8 unknown00[0x2900];
    Band443 bands[3];
    ModelVariantSheet sheets[1];
    u8 unknown2B00[0x33D0 - 0x2B00];
    POLY_GT4 quad;
    u8 unknown3404[0x3570 - 0x3404];
    VECTOR origins[3];
    u8 unknown35A0[0x35C8 - 0x35A0];
    VECTOR directions[3];
    u8 unknown35F8[4];
    s16 screen_x[3], screen_y[3];
    u8 unknown3608[0x3620 - 0x3608];
    s32 frame;
    u32 time;
    u8 unknown3628[0x3634 - 0x3628];
    Bands443Timing *G32 timing;
    u8 unknown3638[0x3678 - 0x3638];
    s16 size;
    u8 unknown367A[0x3698 - 0x367A];
    s32 phase;
} Bands443State;

#endif
