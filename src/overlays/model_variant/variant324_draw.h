#ifndef MEMORIES_MODEL_VARIANT324_DRAW_H
#define MEMORIES_MODEL_VARIANT324_DRAW_H
#include "../spanish_model_variant/variant432_draw.h"

/* Header 324's work area for its one header-432 ring: the ring position is a
 * base plus a velocity scaled by a time value. */
typedef struct {
    u8 unknown_000[0x6CC];
    ModelVariant432Ring rings[1];
    u8 unknown_764[0x684];
    POLY_GT4 quad;
    u8 unknown_E1C[0xBC];
    VECTOR base;
    u8 unknown_EE8[4];
    VECTOR velocity;
    u8 unknown_EFC[0x1C];
    s32 frame;
    u8 unknown_F1C[8];
    s32 step;
    u8 unknown_F28[0x20];
    s32 time;
    u8 unknown_F4C[4];
    s32 state;
} ModelVariant324State;
#endif
