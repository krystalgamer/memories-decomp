#ifndef FRENCH_MODEL_VARIANT412_RINGS_H
#define FRENCH_MODEL_VARIANT412_RINGS_H
#include "../../types.h"
#include "variant402_rings.h"

typedef struct {
    u8 unknown_00[16];
    u16 part_count;
    u16 unknown_12;
    u32 duration;
} Variant412RingConfig;

/* Partial retained-renderer view, not an allocation-size declaration. */
typedef struct {
    u8 unknown_0000[0x12C0];
    Family402Ring rings[2];
    u8 unknown_13E0[0x598];
    POLY_GT4 quad;
    u8 unknown_19AC[0x104];
    MATRIX transform;
    SVECTOR target;
    VECTOR velocity;
    u8 unknown_1AE8[0x1C];
    s32 frame;
    s32 unknown_1B08;
    u32 elapsed;
    s32 animation_frame;
    s32 step;
    u8 unknown_1B18[12];
    Variant412RingConfig *G32 config;
    u8 unknown_1B28[0x20];
    s32 selected_part;
    u8 unknown_1B4C[0x10];
    s16 width;
    s16 interpolation;
    u8 unknown_1B60[4];
    s32 phase;
} Variant412RingView;

void func_8013CEC8(u8 *context);
#endif
