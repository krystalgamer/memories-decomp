#ifndef MEMORIES_DECOMP_GS_SPRITE_WORDS_H
#define MEMORIES_DECOMP_GS_SPRITE_WORDS_H

#include "../types.h"
#include "../psyq/libgs.h"

/* A GsSPRITE that the card-list and Library grid drawers also write a word
 * at a time: w/h, cx/cy and r/g/b with the pad byte after them are one
 * store each, and u/v one halfword. Unlike SpritePrim (ygo_types.h), the
 * union carries the SDK type itself, so `prim` goes to GsSortFastSprite and
 * the other GsSPRITE consumers as it is. */
typedef union {
    GsSPRITE prim;
    struct {
        u32 attribute;
        s16 x;
        s16 y;
        u32 wh;
        u16 tpage;
        u16 uv;
        u32 cxcy;
        u32 rgb;
    } words;
} GsSpriteWords;

typedef char GsSpriteWords_wh_must_be_GsSPRITE_w[
    OFFSETOF(GsSpriteWords, words.wh) == OFFSETOF(GsSPRITE, w) ? 1 : -1
];
typedef char GsSpriteWords_uv_must_be_GsSPRITE_u[
    OFFSETOF(GsSpriteWords, words.uv) == OFFSETOF(GsSPRITE, u) ? 1 : -1
];
typedef char GsSpriteWords_cxcy_must_be_GsSPRITE_cx[
    OFFSETOF(GsSpriteWords, words.cxcy) == OFFSETOF(GsSPRITE, cx) ? 1 : -1
];
typedef char GsSpriteWords_rgb_must_be_GsSPRITE_r[
    OFFSETOF(GsSpriteWords, words.rgb) == OFFSETOF(GsSPRITE, r) ? 1 : -1
];

#endif
