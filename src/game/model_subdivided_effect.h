#ifndef MEMORIES_DECOMP_MODEL_SUBDIVIDED_EFFECT_H
#define MEMORIES_DECOMP_MODEL_SUBDIVIDED_EFFECT_H

#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"

typedef struct {
    u8 colors[3][6];
    s16 minimum_height;
    s16 minimum_radius;
    s16 growth_duration;
    s16 fade_duration;
    s16 pad;
} ModelSubdividedEffectConfig;

typedef struct {
    ModelSubdividedEffectConfig *config;
    SVECTOR vertices[6];
    SVECTOR *vertex_links[24];
    SVECTOR subdivided_vertices[384];
    Triplet colors[6];
    u8 *color_links[24];
    Triplet subdivided_colors[384];
    u8 field_130C;
    u8 field_130D;
    u8 pad_130E[2];
    s32 elapsed;
    s32 remaining;
} ModelSubdividedEffect;

typedef char ModelSubdividedEffectConfig_size_must_be_28[
    sizeof(ModelSubdividedEffectConfig) == 28 ? 1 : -1
];
#if !defined(__SIZEOF_POINTER__) || __SIZEOF_POINTER__ == 4
typedef char ModelSubdividedEffect_size_must_be_0x1318[
    sizeof(ModelSubdividedEffect) == 0x1318 ? 1 : -1
];
#endif

extern VECTOR D_8001185C;
extern char D_8001186C[];

s32 func_8006AF74(ModelSubdividedEffect *data, s32 mode);

#endif
