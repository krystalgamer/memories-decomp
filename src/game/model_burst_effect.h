#ifndef MEMORIES_DECOMP_MODEL_BURST_EFFECT_H
#define MEMORIES_DECOMP_MODEL_BURST_EFFECT_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

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

/* The ring colours `table` points at, D_800916D4 in model_geometry_tables.c:
 * one byte per ring for each channel. */
typedef struct {
    u8 r[3];
    u8 g[3];
    u8 b[3];
} ModelBurstPalette;

/* One texture record of D_80091610 in model_geometry_tables.c: the pixel mode
 * and the frame-buffer rectangles of the image and of its colour table. */
typedef struct {
    u16 mode;
    u16 unk_02;
    RECT prect;
    u32 *paddr;
    RECT crect;
    u32 *caddr;
} ModelBurstImage;

/* The texture page and colour table words built from a ModelBurstImage. */
typedef struct {
    u16 tpage;
    u16 clut;
} ModelBurstTexture;

typedef struct {
    ModelBurstPalette *table;
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
    u8 fade;
    u8 pad_d89;
    u16 tpage;
    u16 clut;
    ModelBurstTexture spark_texture[1];
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

s32 func_8006CD78(void *data, s32 arg1);

#endif
