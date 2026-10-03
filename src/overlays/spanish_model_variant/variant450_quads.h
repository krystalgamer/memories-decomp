#ifndef SPANISH_MODEL_VARIANT450_QUADS_H
#define SPANISH_MODEL_VARIANT450_QUADS_H

#include "../../types.h"
#include "variant450_lines.h"

typedef struct {
    SVECTOR points[16];
    u8 end_color[4];
    u8 color[4];
    s32 scale;
    u8 unknown_8C[12];
    s32 fading;
    s32 brightness;
} Variant450QuadGroup;

typedef struct {
    u8 unknown_00[12];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_14[4];
    u32 fade_start;
    u32 fade_end;
} Variant450QuadConfig;

typedef struct {
    u8 unknown_0000[0x440];
    Variant450Primary primary[6];
    u8 unknown_10D0[0x24C8];
    Variant450QuadGroup groups[7];
    u8 unknown_39F8[0x764];
    POLY_GT4 quad;
    u8 unknown_4190[0xD8];
    VECTOR translation;
    u8 unknown_4278[0x30];
    s32 flags;
    u32 elapsed;
    u8 unknown_42B0[4];
    s32 step;
    u8 unknown_42B8[4];
    Variant450QuadConfig *G32 config;
    u8 unknown_42C0[0x38];
    s32 phase;
} Variant450QuadView;

#endif
