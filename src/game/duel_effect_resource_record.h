#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H

#include "../types.h"
/* libgpu.h uses SVECTOR without declaring it, so libgte.h has to come first
 * for this header to be includable on its own. */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* One 0x40-byte entry of the array at D_800EA0E8. func_800289BC uploads four
 * textures from a single source origin, and casts each of the four
 * destination rectangles to RECT before handing it to LoadImage, so the
 * layout is four RECTs followed by that origin. */
typedef struct {
    /* func_80029528 hands both of these to func_8004036C, which takes a
     * void *object, and then clears them -- so the two leading words are
     * display-object pointers being released, not padding. */
    void *object_00;
    void *object_04;
    RECT rects[4];
    u16 src_x;
    u16 src_y;
    u16 field_2C;
    u16 field_2E;
    /* func_80029164.c:19 writes this one as `*(u16 *)(record + 0x30)`, and
     * the store it produces is `sh $a1, 0x30($v0)` at 80029184, which does
     * not discriminate the sign -- u16 follows that source's own spelling,
     * not a measurement. */
    u16 field_30;
    /* func_80028B08 reads seven fields between 0x32 and 0x3C off a record of
     * this array, and src/candidates_target/func_80028B08.S gives each of
     * them a width and a sign: `lh` at 0x32, 0x34, 0x36 and 0x38, `lbu` at
     * 0x3A, 0x3B and 0x3C, all through the $s3 that 80028D88 forms as
     * &D_800EA0E8[index]. The four halfwords are read as two pairs, each
     * summed and clamped before being encoded as decimal digits; what the
     * pairs are is not recorded here, because a sum and a clamp are not
     * evidence of a name. 0x3D through 0x3F stay unnamed: that function's
     * listing does not read them off the record, and no source under src/
     * names them through this type. */
    s16 field_32;
    s16 field_34;
    s16 field_36;
    s16 field_38;
    u8 field_3A;
    u8 field_3B;
    u8 field_3C;
    u8 pad_3D[3];
} DuelEffectResourceRecord;

typedef char DuelEffectResourceRecord_size_must_be_0x40[
    sizeof(DuelEffectResourceRecord) == 0x40 ? 1 : -1
];

#define DUEL_EFFECT_RESOURCE_RECORD_OFFSET(member) \
    ((u32)&(((DuelEffectResourceRecord *)0)->member))

typedef char DuelEffectResourceRecord_field_30_offset_must_be_0x30[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(field_30) == 0x30 ? 1 : -1
];
typedef char DuelEffectResourceRecord_field_32_offset_must_be_0x32[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(field_32) == 0x32 ? 1 : -1
];
typedef char DuelEffectResourceRecord_field_3A_offset_must_be_0x3A[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(field_3A) == 0x3A ? 1 : -1
];
typedef char DuelEffectResourceRecord_field_3C_offset_must_be_0x3C[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(field_3C) == 0x3C ? 1 : -1
];

/* The array itself. Seven sources declared it identically and all seven
 * already include this header, which exists to describe one of its entries,
 * so this is where it belongs.
 *
 * src/overlays/password/shop.c also names the symbol, spelled u8 [],
 * and is deliberately left alone. It never indexes it -- the one use is
 * `D_8016D430 = D_800EA0E8`, which takes the address only -- so its spelling
 * is not a competing claim about the element type, and it does not include
 * this header. */
extern DuelEffectResourceRecord D_800EA0E8[];

#endif
