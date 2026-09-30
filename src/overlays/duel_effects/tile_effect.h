#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TILE_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TILE_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* One of the two 8-byte descriptors at D_8015A648 that effect id 3 selects
 * by its phase. The North American path uses five by seven 28-pixel tiles
 * from D_8015A62C and lights a sparkle on each. PAL keeps the horizontal
 * geometry but has independently verified vertical-grid/UV constants. */
typedef struct {
    u16 size;               /* 0x00 */
    u16 delay;              /* 0x02 */
    u16 frames;             /* 0x04 */
    u16 texture;            /* 0x06 */
} TileEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    TileEffectDescriptor *G32 descriptor;   /* 0x000 */
    u16 active[5][7];                   /* 0x004 */
    u16 cursor[5];                      /* 0x04A */
    u16 countdown[5];                   /* 0x054 */
    u16 sparkle[5][7];                  /* 0x05E */
    u16 radius[5][7];                   /* 0x0A4 */
    u16 jitter[5][7];                   /* 0x0EA */
    u16 step;                           /* 0x130 */
    u8 pad_132[2];                      /* 0x132 */
    u32 frame;                          /* 0x134 */
    s32 updates;                        /* 0x138 */
    u16 texture_words[2];               /* 0x13C */
    u8 colors_a[5][7][4];               /* 0x140 */
    u8 colors_c[5][7][4];               /* 0x1CC */
    u8 colors_b[5][7][4];               /* 0x258 */
} TileEffectState;

/* The texture-word table D_8015B748 seen as the page/clut pairs the
 * dispatcher packs, one per image descriptor. */
typedef struct {
    u16 page;               /* 0x00 */
    u16 clut;               /* 0x02 */
} TileEffectTextureWords;

extern GsIMAGE D_8015A62C;
extern TileEffectDescriptor D_8015A648[2];

#endif
