#ifndef FRENCH402_RINGS_VIEW_H
#define FRENCH402_RINGS_VIEW_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[4][4];
    s32 scale;
    u8 unknown_84[12];
} Family402Ring;

typedef struct {
    u8 unknown_00[12];
    u16 part_count;
    u16 unknown_0E;
    u32 duration;
} Family402Config;

/* Partial accessed view; entry also uses fields beyond this helper's view. */
typedef struct {
    u8 unknown_000[0x58];
    Family402Ring rings[2];
    u8 unknown_178[0x564];
    POLY_GT4 quad;
    u8 unknown_710[0x104];
    MATRIX transform;
    SVECTOR target;
    VECTOR velocity;
    u8 unknown_84C[0x1C];
    s32 frame;
    u32 elapsed;
    s32 animation_frame;
    s32 step;
    s32 fade;
    Family402Config *G32 config;
    u8 unknown_880[0x10];
    s32 selected_part;
    u8 unknown_894[0x10];
    s16 width;
    s16 interpolation;
    u8 unknown_8A8[4];
    s32 phase;
} Family402RingView;

void func_8013C69C(u8 *context);
#endif
