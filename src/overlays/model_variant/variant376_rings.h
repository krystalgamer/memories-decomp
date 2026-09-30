#ifndef MEMORIES_MODEL_VARIANT376_RINGS_H
#define MEMORIES_MODEL_VARIANT376_RINGS_H
#include "../spanish_model_variant/variant337_rings.h"

/* Header 376's timing record: the header-337 fields, further in. */
typedef struct {
    u8 unknown_00[0x24];
    u32 start;
    u32 end;
    u32 unknown_2C;
    u32 fade_start;
    u32 fade_end;
} ModelVariant376Config;

/* Header 376's work area: the header-337 rings and quad at the same offsets,
 * the rest moved, and a flag choosing one ring or two. */
typedef struct {
    u8 unknown_000[0x3A4];
    ModelVariant337Ring rings[2];
    u8 unknown_4D4[0x764];
    POLY_GT4 quad;
    u8 unknown_C6C[0xB0];
    MATRIX transform;
    SVECTOR target;
    u8 unknown_D44[0x5C];
    s32 frame;
    u32 elapsed;
    u32 unknown_DA8;
    s32 step;
    u32 unknown_DB0;
    ModelVariant376Config *G32 config;
    u8 unknown_DB8[0x10];
    s32 single;
    u8 unknown_DCC[0x2C];
    s32 state;
} ModelVariant376State;
#endif
