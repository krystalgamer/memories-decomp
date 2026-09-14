#ifndef MEMORIES_DECOMP_MODEL_SPARK_EFFECT_H
#define MEMORIES_DECOMP_MODEL_SPARK_EFFECT_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Particle state for func_8006F1B4, the fourth handler in D_800114E8. It
 * holds 32 sparks with their sizes and colours, 32 flashes and 64 dust
 * particles with their speeds and frames. */
typedef struct {
    u16 size;
    u16 grow;
} ModelSparkSize;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad;
} ModelSparkColor;

typedef struct {
    SVECTOR sparks[32];
    ModelSparkSize sizes[32];
    SVECTOR flashes[32];
    SVECTOR dust[64];
    SVECTOR dust_speed[64];
    u16 dust_frame[64];
    u16 spark_count;
    u16 flash_count;
    u16 frame;
    u16 mode;
    u8 fade;
    u8 pad709;
    u16 tpage;
    u16 clut;
    ModelSparkColor spark_colors[32];
    ModelSparkColor flash_colors[32];
    u8 dust_r;
    u8 dust_g;
    u8 dust_b;
} ModelSparkEffect;

#endif
