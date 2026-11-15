#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TILE_EFFECT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TILE_EFFECT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* One of the two 8-byte descriptors at gDuelEffect_DestroyFlamesConfigs that effect id 3 selects
 * by its phase. The North American path uses five by seven 28-pixel tiles
 * from gDuelEffect_DestroyFlamesTextureImage and lights a sparkle on each. PAL keeps the horizontal
 * geometry but has independently verified vertical-grid/UV constants. */
typedef struct {
    u16 size;               /* 0x00 */
    u16 delay;              /* 0x02 */
    u16 frames;             /* 0x04 */
    u16 texture;            /* 0x06 */
} TileEffectDescriptor;

/* The effect's working state in the request buffer. */
typedef struct {
    TileEffectDescriptor *descriptor;   /* 0x000 */
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
    CVECTOR colors_a[5][7];             /* 0x140 */
    CVECTOR colors_c[5][7];             /* 0x1CC */
    CVECTOR colors_b[5][7];             /* 0x258 */
} TileEffectState;

#define TILE_EFFECT_STATE_OFFSET(member) ((u32)&(((TileEffectState *)0)->member))
typedef char TileEffectState_colour_offsets_must_match[
    TILE_EFFECT_STATE_OFFSET(colors_a) == 0x140 &&
    TILE_EFFECT_STATE_OFFSET(colors_c) == 0x1CC &&
    TILE_EFFECT_STATE_OFFSET(colors_b) == 0x258 ? 1 : -1
];
#undef TILE_EFFECT_STATE_OFFSET

/* The texture-word table gDuelEffect_TexturePageClutPairs seen as the page/clut pairs the
 * dispatcher packs, one per image descriptor. */
typedef struct {
    u16 page;               /* 0x00 */
    u16 clut;               /* 0x02 */
} TileEffectTextureWords;

#endif
