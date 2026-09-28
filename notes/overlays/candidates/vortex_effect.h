/* Types for the vortex_effect.c candidate beside this file; see its header. */
#ifndef MEMORIES_DECOMP_DUEL_EFFECT_VORTEX_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_VORTEX_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../game/display_object.h"

/* One of the two 16-byte descriptors at D_8015A60C that effect id 0x11
 * selects by its phase: two colours, the rotating quad's size, two spin
 * rates, a radius and the mode that decides which field cards it pulls. */
typedef struct {
    u8 red;                 /* 0x00 */
    u8 green;               /* 0x01 */
    u8 blue;                /* 0x02 */
    u8 red_b;               /* 0x03 */
    u8 green_b;             /* 0x04 */
    u8 blue_b;              /* 0x05 */
    u16 size;               /* 0x06 */
    u16 spin;               /* 0x08 */
    u16 radius;             /* 0x0A */
    u16 rate;               /* 0x0C */
    u16 mode;               /* 0x0E */
} VortexEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    VortexEffectDescriptor *descriptor; /* 0x0000 */
    SVECTOR position;                   /* 0x0004 */
    SVECTOR moves[20];                  /* 0x000C */
    SVECTOR spins[20];                  /* 0x00AC */
    SVECTOR trails[20][16];             /* 0x014C */
    SVECTOR sparks[64];                 /* 0x0B4C */
    SVECTOR spark_velocities[64];       /* 0x0D4C */
    SVECTOR curves[48][8];              /* 0x0F4C */
    SVECTOR curve_positions[48];        /* 0x1B4C */
    SVECTOR center;                     /* 0x1CCC */
    u16 curve_angles[48];               /* 0x1CD4 */
    u16 curve_states[48];               /* 0x1D34 */
    u16 spin;                           /* 0x1D94 */
    u16 frames;                         /* 0x1D96 */
    u16 stage;                          /* 0x1D98 */
    u16 card_flags[20];                 /* 0x1D9A */
    u16 fade;                           /* 0x1DC2 */
    u16 card_count;                     /* 0x1DC4 */
    u16 cards_active;                   /* 0x1DC6 */
    u16 spark_count;                    /* 0x1DC8 */
    u16 curve_count;                    /* 0x1DCA */
    u16 card_steps[20];                 /* 0x1DCC */
    u16 arrived;                        /* 0x1DF4 */
    u16 step;                           /* 0x1DF6 */
    u16 curve_steps[48];                /* 0x1DF8 */
    u8 color[4];                        /* 0x1E58 */
    u8 color_b[4];                      /* 0x1E5C */
    u8 spark_colors[64][4];             /* 0x1E60 */
    u8 curve_colors[48][4];             /* 0x1F60 */
} VortexEffectState;

/* The update's stack frame, in the order the frame lays it out. */
typedef struct {
    MATRIX world;           /* 0x00 */
    MATRIX saved;           /* 0x20 */
    SVECTOR rotation;       /* 0x40 */
    VECTOR scale;           /* 0x48 */
    POLY_GT4 polygon;       /* 0x58 */
    SVECTOR quad[4];        /* 0x90 */
    u8 half[4];             /* 0xB0 */
} VortexEffectFrame;

/* The texture-word table D_8015B748 seen as page/clut pairs. */
typedef struct {
    u16 page;               /* 0x00 */
    u16 clut;               /* 0x02 */
} VortexEffectTextureWords;

extern VortexEffectDescriptor D_8015A60C[2];
extern VECTOR D_80146034;
extern DisplayObject *D_8015B7A0[21];

#endif
