#ifndef MEMORIES_DECOMP_DUEL_EFFECT_GATHER_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_GATHER_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"

/* The single descriptor at D_8015A5F8 that effect id 14 reads. Every field
 * is named for the call argument or store func_801481A8 reads it into; the
 * effect's name follows its particle velocities, which point at the
 * origin. */
typedef struct {
    u16 spread;             /* 0x00 */
    u16 size;               /* 0x02 */
    u16 frames;             /* 0x04 */
    u16 ring_radius[3];     /* 0x06 */
    u16 curve_size;         /* 0x0C */
    u16 widths[2];          /* 0x0E */
    u16 height;             /* 0x12 */
} GatherEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    GatherEffectDescriptor *G32 descriptor; /* 0x000 */
    SVECTOR particles[32];              /* 0x004 */
    SVECTOR velocities[32];             /* 0x104 */
    SVECTOR curves[2][4];               /* 0x204 */
    SVECTOR rings[3][32];               /* 0x244 */
    SVECTOR rotations[16];              /* 0x544 */
    SVECTOR particles_b[48];            /* 0x5C4 */
    SVECTOR velocities_b[48];           /* 0x744 */
    u16 sprite_u;                       /* 0x8C4 */
    u16 sprite_v;                       /* 0x8C6 */
    u16 tpage;                          /* 0x8C8 */
    u16 clut;                           /* 0x8CA */
    u16 sizes[32];                      /* 0x8CC */
    u16 flags[32];                      /* 0x90C */
    u16 done;                           /* 0x94C */
    u8 pad_94E[2];                      /* 0x94E */
    s32 frame;                          /* 0x950 */
    u16 particle_count;                 /* 0x954 */
    u16 particle_b_count;               /* 0x956 */
    u16 scale;                          /* 0x958 */
    u16 step;                           /* 0x95A */
    u8 colors[32][4];                   /* 0x95C */
    u8 color[4];                        /* 0x9DC */
    u8 colors_b[48][4];                 /* 0x9E0 */
} GatherEffectState;

/* The update's stack frame: the matrix, packet, vectors and quad it hands
 * the helpers, in the order the frame lays them out. */
typedef struct {
    MATRIX world;           /* 0x00 */
    POLY_FT4 polygon;       /* 0x20 */
    SVECTOR rotation;       /* 0x48 */
    SVECTOR position;       /* 0x50 */
    VECTOR scale;           /* 0x58 */
    SVECTOR quad[4];        /* 0x68 */
} GatherEffectFrame;

extern GatherEffectDescriptor D_8015A5F8;
extern VECTOR D_80146024;

#endif
