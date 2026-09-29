#ifndef FRENCH_SPECIAL_SECOND_RING_H
#define FRENCH_SPECIAL_SECOND_RING_H
#include "../../types.h"
#include "ring.h"

typedef struct {
    u8 prefix[16];
    u32 start, ramp_end, field_18, second_start;
} ExodiaSecondRingTiming;

typedef struct {
    u8 prefix[0x308];
    ExodiaRing rings[2];
    u8 gap_438[0xAF4 - 0x438];
    POLY_GT4 quad;
    u8 gap_B28[0xC2C - 0xB28];
    SVECTOR origin;
    VECTOR direction;
    u8 gap_C44[0xC60 - 0xC44];
    u32 frame_count;
    u32 frame;
    u8 gap_C68[0xC74 - 0xC68];
    ExodiaSecondRingTiming *timing;
    u8 gap_C78[0xC98 - 0xC78];
    s16 distance;
    u8 gap_C9A[0xCA4 - 0xC9A];
    s32 phase;
} ExodiaSecondRingState;

void func_8017B9B0(u8 *context);
#endif
