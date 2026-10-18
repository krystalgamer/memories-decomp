#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_RECORD_H

#include "../types.h"
/* libgpu.h uses SVECTOR without declaring it, so libgte.h has to come first
 * for this header to be includable on its own. */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

struct DisplayObject;

/* One 0x40-byte entry of the array at D_800EA0E8. func_800289BC uploads four
 * textures from a single source origin, and casts each of the four
 * destination rectangles to RECT before handing it to LoadImage, so the
 * layout is four RECTs followed by that origin. */
typedef struct DuelEffectResourceRecord {
    /* func_800291E0 acquires both objects from the display-object pool,
     * publishes the allocated-second primary at +0 and the secondary at +4,
     * and links the primary from the secondary. Clear/release helpers keep
     * the same nullable pointer values and order. */
    struct DisplayObject *G32 object_00;
    struct DisplayObject *G32 object_04;
    RECT rects[4];
    u16 src_x;
    u16 src_y;
    u16 field_2C;
    u16 field_2E;
    /* func_80029164 stores into this one, and the store is
     * `sh $a1, 0x30($v0)` at 80029184, which does not discriminate the sign.
     * u16 follows the `*(u16 *)(record + 0x30)` cast that source carried when
     * the field was named. func_800291E0 explicitly takes a signed view for
     * its one-based card index, matching that constructor's lh. */
    u16 field_30;
    /* The constructor supplies ATK/DEF overrides or decoded base stats;
     * CardPreview_DrawDetailPanel sends the corresponding sums to its ATK
     * and DEF digit rows. Retail uses lh at 0x32, 0x34, 0x36 and 0x38,
     * and lbu at 0x3A, 0x3B and 0x3C, through the resource base formed at
     * 80028D88. Keep the separate adjustment and byte members offset-named
     * until their writer contracts are recovered. 0x3D..0x3F remain
     * padding: no recovered member access names those bytes. */
    s16 attack;
    s16 defense;
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

typedef char DuelEffectResourceRecord_object_00_offset_must_be_0x0[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(object_00) == 0 ? 1 : -1
];
typedef char DuelEffectResourceRecord_object_04_offset_must_be_0x4[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(object_04) == 4 ? 1 : -1
];
typedef char DuelEffectResourceRecord_rects_offset_must_be_0x8[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(rects) == 8 ? 1 : -1
];
typedef char DuelEffectResourceRecord_field_30_offset_must_be_0x30[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(field_30) == 0x30 ? 1 : -1
];
typedef char DuelEffectResourceRecord_attack_offset_must_be_0x32[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(attack) == 0x32 ? 1 : -1
];
typedef char DuelEffectResourceRecord_defense_offset_must_be_0x34[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(defense) == 0x34 ? 1 : -1
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
 * src/overlays/password/shop.c includes it too. It never indexes the array:
 * Password_InitShopScreen parks the first record in D_8016D430 and sets its
 * source rectangle through that pointer, which module_state.h declares by
 * this header's struct tag. */
extern DuelEffectResourceRecord D_800EA0E8[];

#endif
