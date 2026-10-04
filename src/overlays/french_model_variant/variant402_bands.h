#ifndef FRENCH402_BANDS_VIEW_H
#define FRENCH402_BANDS_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 scale;
    s32 cycles;
} Family402Band;

void func_8013CB38(u8 *context);

#ifdef MODEL_VARIANT412_BANDS
typedef POLY_FT4 Family402BandPacket;
#else
typedef POLY_GT4 Family402BandPacket;
#endif

enum {
#ifdef MODEL_VARIANT412_BANDS
    BAND_WORK_0438 = 0x16A0,
    BAND_WORK_0844 = 0x1AE0,
    BAND_WORK_083C = 0x1AD8,
    BAND_WORK_0840 = 0x1ADC,
    BAND_WORK_0744 = 0x1A64,
    BAND_WORK_0868 = 0x1B04,
    BAND_WORK_0834 = 0x1AD0,
    BAND_WORK_0836 = 0x1AD2,
    BAND_WORK_0838 = 0x1AD4,
    BAND_WORK_0894 = 0x1B4C,
    BAND_WORK_0895 = 0x1B4D,
    BAND_WORK_0896 = 0x1B4E,
    BAND_WORK_0898 = 0x1B50,
    BAND_WORK_0899 = 0x1B51,
    BAND_WORK_089A = 0x1B52,
    BAND_WORK_0874 = 0x1B14,
    BAND_WORK_08AC = 0x1B64,
#else
    BAND_WORK_0438 = 0x438,
    BAND_WORK_0844 = 0x844,
    BAND_WORK_083C = 0x83C,
    BAND_WORK_0840 = 0x840,
    BAND_WORK_0744 = 0x744,
    BAND_WORK_0868 = 0x868,
    BAND_WORK_0834 = 0x834,
    BAND_WORK_0836 = 0x836,
    BAND_WORK_0838 = 0x838,
    BAND_WORK_0894 = 0x894,
    BAND_WORK_0895 = 0x895,
    BAND_WORK_0896 = 0x896,
    BAND_WORK_0898 = 0x898,
    BAND_WORK_0899 = 0x899,
    BAND_WORK_089A = 0x89A,
    BAND_WORK_0874 = 0x874,
    BAND_WORK_08AC = 0x8AC,
#endif
};
#endif
