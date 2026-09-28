/* Types for the shower_effect.c candidate beside this file; see its header. */
#ifndef MEMORIES_DECOMP_DUEL_EFFECT_SHOWER_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_SHOWER_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* One of the six 14-byte descriptors at D_8015AB14 that effect id 10
 * selects by its phase: a colour, two rise parameters for the pair of
 * quads, the particle speed and the drawing mode. */
typedef struct {
    u8 red;                 /* 0x00 */
    u8 green;               /* 0x01 */
    u8 blue;                /* 0x02 */
    u8 pad_03;              /* 0x03 */
    u16 rise_step;          /* 0x04 */
    u16 rise_max;           /* 0x06 */
    u16 speed;              /* 0x08 */
    u16 mode;               /* 0x0A */
    u16 unk_0C;             /* 0x0C */
} ShowerEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    ShowerEffectDescriptor *descriptor; /* 0x000 */
    SVECTOR quads[2][4];                /* 0x004 */
    SVECTOR vertices[4];                /* 0x044 */
    SVECTOR vectors[64];                /* 0x064 */
    SVECTOR drops[64];                  /* 0x264 */
    u16 speeds[64];                     /* 0x464 */
    u16 timers[64];                     /* 0x4E4 */
    u16 rise;                           /* 0x564 */
    u16 stage;                          /* 0x566 */
    u16 done_a;                         /* 0x568 */
    u16 done_b;                         /* 0x56A */
    u16 count;                          /* 0x56C */
    u8 pad_56E[2];                      /* 0x56E */
    s32 frame;                          /* 0x570 */
    s32 updates;                        /* 0x574 */
    u16 step;                           /* 0x578 */
    u8 color[4];                        /* 0x57A */
    u8 color_b[4];                      /* 0x57E */
    u8 color_c[4];                      /* 0x582 */
    u8 colors[64][4];                   /* 0x586 */
    u8 color_d[4];                      /* 0x686 */
} ShowerEffectState;

/* The update's stack frame, in the order the frame lays it out. */
typedef struct {
    MATRIX world;           /* 0x00 */
    MATRIX saved;           /* 0x20 */
    POLY_FT4 polygon;       /* 0x40 */
    SVECTOR rotation;       /* 0x68 */
    SVECTOR position;       /* 0x70 */
    VECTOR scale;           /* 0x78 */
    SVECTOR quad[4];        /* 0x88 */
} ShowerEffectFrame;

/* The texture-word table D_8015B748 seen as page/clut pairs. */
typedef struct {
    u16 page;               /* 0x00 */
    u16 clut;               /* 0x02 */
} ShowerEffectTextureWords;

extern ShowerEffectDescriptor D_8015AB14[6];
extern GsIMAGE D_8015A8C8[21];
extern VECTOR D_80146138;
extern u16 D_8015B778;

#endif
