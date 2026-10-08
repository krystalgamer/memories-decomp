#ifndef FRENCH449_BANDS_VIEW_H
#define FRENCH449_BANDS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[3];
    SVECTOR b[3];
    SVECTOR c[3];
    PSXLONG sa[3];
    PSXLONG sb[3];
    PSXLONG sc[3];
    s32 drift[3];
    u8 ca[3][4];
    u8 cb[3][4];
    u8 unknown_90[0x18];
    s32 otz[3];
} Variant449Band;

typedef struct {
#ifdef MODEL_VARIANT437_BANDS
    u8 unknown_0000[0x888];
#else
    u8 unknown_0000[0xE88];
#endif
    Variant449Band bands[1];
    u8 unknown_0F3C[0xAB4];
    POLY_GT4 quads[2];
    u8 unknown_1A58[0xBC];
    MATRIX origin;
    u8 unknown_1B34[0x5C];
    s32 factor;
    u8 unknown_1B94[4];
    s32 scale;
    s32 spread;
    u8 unknown_1BA0[0x10];
    VECTOR delta;
    s16 axis_x;
    s16 axis_y;
    u8 unknown_1BC4[0x18];
    s32 flags;
    u8 unknown_1BE0[0x6C];
    s16 direction;
} Variant449BandView;

void func_8013D458(u8 *context);
#endif
