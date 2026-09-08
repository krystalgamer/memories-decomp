#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H

#include "../types.h"
#include "../psyq/libgpu.h"

/* One 0x40-byte entry of the array at D_800EA0E8. func_800289BC uploads four
 * textures from a single source origin, and casts each of the four
 * destination rectangles to RECT before handing it to LoadImage, so the
 * layout is four RECTs followed by that origin. */
typedef struct {
    u8 pad_00[8];
    RECT rects[4];
    u16 src_x;
    u16 src_y;
    u16 field_2C;
    u16 field_2E;
    u8 pad_30[0x10];
} DuelEffectResourceRecord;

typedef char DuelEffectResourceRecord_size_must_be_0x40[
    sizeof(DuelEffectResourceRecord) == 0x40 ? 1 : -1
];

#endif
