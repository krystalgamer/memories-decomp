/* Types for the trap_effect.c candidate beside this file; see its header. */
#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TRAP_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TRAP_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"

/* One of the six 38-byte descriptors at D_8015A514 that effect id 8, the
 * request duel_trap_resolution.c creates, selects by its phase. Every field is
 * named for the store or call argument func_80147B18 reads it into. */
typedef struct {
    u8 red;                 /* 0x00 */
    u8 green;               /* 0x01 */
    u8 blue;                /* 0x02 */
    u8 pad_03;              /* 0x03 */
    u16 width;              /* 0x04 */
    u16 width_step;         /* 0x06 */
    u16 width_min;          /* 0x08 */
    u16 height;             /* 0x0A */
    u16 height_step;        /* 0x0C */
    u16 height_max;         /* 0x0E */
    u16 curve_step;         /* 0x10 */
    u16 line_count;         /* 0x12 */
    u16 line_spread;        /* 0x14 */
    u16 line_width;         /* 0x16 */
    u16 particle_speed;     /* 0x18 */
    u16 particle_count;     /* 0x1A */
    u16 has_rings;          /* 0x1C */
    u16 ring_radius[3];     /* 0x1E */
    u16 ring_angle_step;    /* 0x24 */
} TrapEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    TrapEffectDescriptor *descriptor;   /* 0x000 */
    SVECTOR lines[32];                  /* 0x004 */
    SVECTOR particles[64];              /* 0x104 */
    SVECTOR velocities[64];             /* 0x304 */
    SVECTOR rings[3][32];               /* 0x504 */
    SVECTOR position;                   /* 0x804 */
    u16 width;                          /* 0x80C */
    u16 height;                         /* 0x80E */
    u32 scale;                          /* 0x810 */
    s32 frame;                          /* 0x814 */
    u16 step;                           /* 0x818 */
    CVECTOR color;                      /* 0x81A */
} TrapEffectState;

extern TrapEffectDescriptor D_8015A514[6];
extern VECTOR D_80146014;

#endif
