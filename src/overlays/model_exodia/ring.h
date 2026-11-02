#ifndef FRENCH_SPECIAL_RING_H
#define FRENCH_SPECIAL_RING_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/screen_projection.h"

typedef struct {
    u8 prefix[28];
    u32 start, ramp_end, growth_end, shrink_end, end;
} ExodiaRingTiming;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 scale;
    u8 tail[12];
} ExodiaRing;

typedef struct {
    u8 prefix[0x1C0];
    ExodiaRing rings[1];
    u8 gap_258[0x2CC - 0x258];
    POLY_GT4 quad;
    u8 gap_300[0x3F0 - 0x300];
    VECTOR origin;
    u8 gap_400[0x450 - 0x400];
    u32 frame_count;
    u32 frame;
    u32 field_458;
    s32 step;
    u32 field_460;
    ExodiaRingTiming *timing;
    u8 gap_468[0x4DC - 0x468];
    s32 grown;
} ExodiaRingState;

void func_8013B860(u8 *context);
#endif
