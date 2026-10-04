#ifndef FRENCH402_STRIP_VIEW_H
#define FRENCH402_STRIP_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant402_rings.h"

typedef struct {
    SVECTOR points[3][2];
    PSXLONG projected[3][2];
    u8 unknown_48[8];
    s32 depth[2];
} Family402Strip;

void func_8013BA5C(u8 *context);

enum {
#ifdef MODEL_VARIANT412_STRIP
    STRIP_WORK_084E = 0x1AEA,
    STRIP_WORK_084C = 0x1AE8,
    STRIP_WORK_06A8 = 0x1944,
    STRIP_WORK_08AC = 0x1B64,
    STRIP_WORK_08A4 = 0x1B5C,
    STRIP_WORK_0828 = 0x1AC4,
    STRIP_WORK_082C = 0x1AC8,
    STRIP_WORK_0830 = 0x1ACC,
    STRIP_WORK_083C = 0x1AD8,
    STRIP_WORK_0840 = 0x1ADC,
    STRIP_WORK_0844 = 0x1AE0,
    STRIP_WORK_08A6 = 0x1B5E,
    STRIP_WORK_0890 = 0x1B48,
    STRIP_WORK_0874 = 0x1B14,
    STRIP_RECORD_BASE = 0x1268
#else
    STRIP_WORK_084E = 0x84E,
    STRIP_WORK_084C = 0x84C,
    STRIP_WORK_06A8 = 0x6A8,
    STRIP_WORK_08AC = 0x8AC,
    STRIP_WORK_08A4 = 0x8A4,
    STRIP_WORK_0828 = 0x828,
    STRIP_WORK_082C = 0x82C,
    STRIP_WORK_0830 = 0x830,
    STRIP_WORK_083C = 0x83C,
    STRIP_WORK_0840 = 0x840,
    STRIP_WORK_0844 = 0x844,
    STRIP_WORK_08A6 = 0x8A6,
    STRIP_WORK_0890 = 0x890,
    STRIP_WORK_0874 = 0x874,
    STRIP_RECORD_BASE = 0
#endif
};
#endif
