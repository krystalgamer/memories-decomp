#ifndef MEMORIES_MODEL_VARIANT321_RINGS_H
#define MEMORIES_MODEL_VARIANT321_RINGS_H
#include "../spanish_model_variant/variant337_rings.h"

/* Header 321's timing record. */
typedef struct {
    u8 unknown_00[0x20];
    u32 start;
    u32 end;
    u32 unknown_28;
    u32 fade_start;
    u32 fade_end;
} ModelVariant321Config;

/* Header 321's work area: the header-337 rings and quad further in. */
typedef struct {
    u8 unknown_0000[0x1234];
    ModelVariant337Ring rings[2];
    u8 unknown_1364[0xA7C];
    POLY_GT4 quad;
    u8 unknown_1E14[0xB0];
    MATRIX transform;
    u8 unknown_1EE4[0x10];
    SVECTOR target;
    u8 unknown_1EFC[0x2C];
    s32 frame;
    u32 elapsed;
    u32 unknown_1F30;
    s32 step;
    u32 unknown_1F38;
    ModelVariant321Config *G32 config;
    u8 unknown_1F40[0x28];
    s32 state;
} ModelVariant321State;
#endif
