#ifndef MEMORIES_DECOMP_MODEL_BURST_EFFECT_H
#define MEMORIES_DECOMP_MODEL_BURST_EFFECT_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Particle state for func_8006CD78, a handler in the D_800114E8 model effect
 * table. Three expanding rings, 64 dust particles, 32 sparks with drift and
 * rise vectors, 32 embers and 64 smoke particles, each with its frame and
 * colour. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad;
} ModelBurstColor;

typedef struct {
    u8 *table;
    s32 frame;
    SVECTOR rings[3];
    SVECTOR ring_speed[3];
    SVECTOR dust[64];
    SVECTOR dust_speed[64];
    SVECTOR sparks[32];
    SVECTOR spark_drift[32];
    SVECTOR spark_rise[32];
    SVECTOR embers[32];
    SVECTOR smoke[64];
    SVECTOR smoke_speed[64];
    SVECTOR origin;
    u16 dust_frame[64];
    u16 spark_frame[32];
    u16 smoke_frame[64];
    u16 radius;
    u16 phase;
    u16 stage;
    u16 spark_count;
    s8 fade;
    u8 pad_d89;
    u16 tpage;
    u16 clut;
    u16 spark_tpage;
    u16 spark_clut;
    ModelBurstColor colors[3];
    u8 flash_r;
    u8 flash_g;
    u8 flash_b;
    u8 pad_da1;
    ModelBurstColor spark_colors[32];
    u8 smoke_r;
    u8 smoke_g;
    u8 smoke_b;
} ModelBurstEffect;

#endif
